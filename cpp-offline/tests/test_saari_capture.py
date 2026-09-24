"""End-to-end dataset checks. Python stdlib only; not used by the exporter."""
import csv
import json
import math
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import zlib


def dot(a, b):
    return sum(x * y for x, y in zip(a, b))


def cross(a, b):
    return (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0])


def normalize(v):
    length = math.sqrt(dot(v, v))
    return [x / length for x in v]


def read_png(path, width, height):
    data = path.read_bytes()
    assert data[:8] == b'\x89PNG\r\n\x1a\n'
    pos, compressed, ended = 8, bytearray(), False
    while pos < len(data):
        length = struct.unpack_from('>I', data, pos)[0]
        kind = data[pos+4:pos+8]
        payload = data[pos+8:pos+8+length]
        crc = struct.unpack_from('>I', data, pos+8+length)[0]
        assert zlib.crc32(kind+payload) & 0xffffffff == crc
        if kind == b'IHDR':
            assert struct.unpack('>IIBBBBB', payload) == (width, height, 8, 2, 0, 0, 0)
        elif kind == b'IDAT':
            compressed.extend(payload)
        elif kind == b'IEND':
            ended = True
        pos += 12 + length
    assert ended and pos == len(data)
    pixels = zlib.decompress(compressed)
    assert len(pixels) == height * (1 + width*3)
    assert all(pixels[y*(1+width*3)] <= 4 for y in range(height))


def read_model(directory):
    cameras, images, points = {}, {}, {}
    for line in (directory/'cameras.txt').read_text().splitlines():
        if not line or line.startswith('#'):
            continue
        v = line.split()
        assert len(v) == 8 and v[1] == 'PINHOLE'
        cameras[int(v[0])] = (int(v[2]), int(v[3]), *map(float, v[4:]))
    lines = iter((directory/'images.txt').read_text().splitlines())
    for line in lines:
        if not line or line.startswith('#'):
            continue
        v = line.split()
        assert len(v) == 10
        q = list(map(float, v[1:5]))
        assert abs(dot(q, q)-1) < 1e-12
        w, x, y, z = q
        rotation = [(1-2*(y*y+z*z), 2*(x*y-z*w), 2*(x*z+y*w)),
                    (2*(x*y+z*w), 1-2*(x*x+z*z), 2*(y*z-x*w)),
                    (2*(x*z-y*w), 2*(y*z+x*w), 1-2*(x*x+y*y))]
        assert abs(dot(rotation[0], cross(rotation[1], rotation[2]))-1) < 1e-12
        obs = next(lines).split()
        assert len(obs) % 3 == 0
        images[int(v[0])] = dict(rotation=rotation, translation=list(map(float, v[5:8])),
                                 camera=int(v[8]), name=v[9],
                                 observations=[(float(obs[i]), float(obs[i+1]), int(obs[i+2]))
                                               for i in range(0, len(obs), 3)])
    for line in (directory/'points3D.txt').read_text().splitlines():
        if not line or line.startswith('#'):
            continue
        v = line.split()
        assert len(v) >= 10 and (len(v)-8) % 2 == 0
        assert all(0 <= int(c) <= 255 for c in v[4:7])
        points[int(v[0])] = (list(map(float, v[1:4])),
                             [(int(v[i]), int(v[i+1])) for i in range(8, len(v), 2)])
    return cameras, images, points


def verify_dataset(root):
    meta = json.loads((root/'capture.json').read_text())
    assert meta['status'] == 'complete' and meta['scene_time_seconds'] == 30
    rows = list(csv.DictReader((root/'manifest.csv').open()))
    path = list(csv.DictReader((root/'camera_path.csv').open()))
    assert len(rows) == len(path) == meta['view_count']
    assert meta['points'] > 100 and meta['meditate_points'] > 0 and meta['klunssi_points'] > 0
    center, radius = meta['hemisphere_center_native'], meta['hemisphere_radius']
    for row, pose in zip(rows, path):
        position = [float(pose[k]) for k in ('px', 'py', 'pz')]
        assert position[2] >= meta['sea_z']+meta['sea_margin']
        assert abs(math.dist(position, center)-radius) < 1e-4
        read_png(root/row['file'], meta['width'], meta['height'])
    train_ids = set()
    validation_ids = set()
    total_error = 0
    for split, prefix in [('train', root), ('validation', root/'validation')]:
        if not prefix.exists():
            continue
        cameras, images, points = read_model(prefix/'sparse')
        expected = {int(row['image_id']) for row in rows if row['split'] == split}
        assert set(images) == expected
        (train_ids if split == 'train' else validation_ids).update(expected)
        if split == 'train':
            assert len(points) == meta['points']
            assert all(len(track) >= 2 for _, track in points.values())
        for image_id, view in images.items():
            assert (prefix/'images'/view['name']).exists()
            rotation, translation = view['rotation'], view['translation']
            camera_center = [-sum(rotation[r][c]*translation[r] for r in range(3)) for c in range(3)]
            source = path[image_id-1]
            native_center = [float(source[k]) for k in ('px','py','pz')]
            assert math.dist(camera_center, [-native_center[0], *native_center[1:]]) < 1e-8
            target = [float(source[k]) for k in ('tx','ty','tz')]
            forward = normalize([a-b for a,b in zip(target,native_center)])
            right = normalize(cross([0,0,1],forward))
            up = cross(forward,right)
            width, height, fx, fy, cx, cy = cameras[view['camera']]
            assert abs(fx-width/2/math.tan(math.radians(float(source['hfov_degrees']))/2)) < 0.001
            for observation_id, (x, y, point_id) in enumerate(view['observations']):
                world, track = points[point_id]
                assert (image_id, observation_id) in track
                projected = [dot(axis, world)+t for axis,t in zip(rotation,translation)]
                assert projected[2] > 0
                error = math.hypot(cx+fx*projected[0]/projected[2]-x,
                                   cy+fy*projected[1]/projected[2]-y)
                assert error < 1e-7
                total_error = max(total_error,error)
                # Independent native projection checks handedness and camera target.
                native_world = [-world[0], *world[1:]]
                relative = [a-b for a,b in zip(native_world,native_center)]
                native_x = cx + fx*dot(relative,right)/dot(relative,forward)
                native_y = cy - fy*dot(relative,up)/dot(relative,forward)
                assert math.hypot(native_x-x,native_y-y) < 0.001
                assert 0 <= x < width and 0 <= y < height
        for point_id, (_, track) in points.items():
            assert len({image for image, _ in track}) == len(track)
            for image, observation in track:
                assert images[image]['observations'][observation][2] == point_id
    assert not train_ids & validation_ids
    print(f'Validated {len(rows)} PNGs, {meta["points"]} points; reprojection error {total_error:.3g} px')


def main():
    executable, workspace = map(lambda p: Path(p).resolve(), sys.argv[1:3])
    with tempfile.TemporaryDirectory(prefix='saari-gsplat-') as temp:
        base = Path(temp)

        def run(output, *args, success=True):
            result = subprocess.run([str(executable), '--sequence', 'saari-gsplat', '--frames', '36',
                                     '--width','320','--height','240','--output',str(output), *args],
                                    cwd=workspace, capture_output=True, text=True)
            assert (result.returncode == 0) == success, result.stdout + result.stderr
            return result

        first = base/'first'
        run(first)
        verify_dataset(first)
        replay = base/'replay'
        run(replay, '--gsplat-camera-path', str(first/'camera_path.csv'), '--fps','29')
        for path in first.rglob('*'):
            if path.is_file() and path.suffix in ('.png','.csv','.txt'):
                assert path.read_bytes() == (replay/path.relative_to(first)).read_bytes(), path
        # Repeated identical views must stay byte-identical despite frame index changes.
        csv_lines = (first/'camera_path.csv').read_text().splitlines()
        duplicate_csv = base/'duplicate.csv'
        duplicate_csv.write_text('\n'.join([csv_lines[0]]+[csv_lines[1]]*12)+'\n')
        duplicate = base/'duplicate'
        run(duplicate, '--gsplat-camera-path',str(duplicate_csv),'--gsplat-validation-every','0')
        duplicate_images = list((duplicate/'images').glob('*.png'))
        assert len(duplicate_images) == 12
        assert len({p.read_bytes() for p in duplicate_images}) == 1
        assert not (duplicate/'validation').exists()
        run(first, success=False)  # Do not overwrite a dataset.
        for i, args in enumerate([('--gsplat-time','nan'), ('--gsplat-radius','1'),
                                  ('--gsplat-validation-every','1'), ('--width','4097'),
                                  ('--until-song-position','0x100')]):
            output = base/f'invalid-{i}'
            run(output,*args,success=False)
            assert not output.exists()
        bad_csv = base/'underwater.csv'
        fields = csv_lines[1].split(',')
        fields[2] = '-1'
        bad_csv.write_text('\n'.join([csv_lines[0], ','.join(fields)] + csv_lines[2:])+'\n')
        run(base/'underwater','--gsplat-camera-path',str(bad_csv),success=False)
        assert not (base/'underwater').exists()
        print('CSV replay, frozen scene, holdout isolation, PNG integrity and invalid-input checks passed.')


if __name__ == '__main__':
    main()
