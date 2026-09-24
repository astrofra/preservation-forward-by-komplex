"""Maku dataset integration checks; Python stdlib is only a test dependency."""
import csv
import json
import math
from pathlib import Path
import re
import subprocess
import sys
import tempfile

from test_saari_capture import cross, dot, normalize, read_model, read_png


def rows(path):
    with path.open() as stream:
        return list(csv.DictReader(stream))


def ase_tracks(workspace):
    source = (workspace/'original/forward/asses/vuori5.ase').read_text()
    tracks = {}
    for name, body in re.findall(r'\*TM_ANIMATION\s*{\s*\*NODE_NAME "([^"]+)"'
                                r'\s*\*CONTROL_POS_TRACK\s*{([^}]+)', source):
        tracks[name] = [(int(t), list(map(float, (x, y, z)))) for t, x, y, z in
                        re.findall(r'\*CONTROL_POS_SAMPLE\s+(\d+)\s+(\S+)\s+(\S+)\s+(\S+)', body)]
    assert set(tracks) == {'Camera01', 'Camera01.Target'}
    return tracks


def spline(track, seconds):
    """Independent double-precision Catmull-Rom evaluation of the source ASE."""
    tick = (seconds*1000) % track[-1][0]
    j = next(i for i in range(1, len(track)) if tick <= track[i][0])
    t = (tick-track[j-1][0])/(track[j][0]-track[j-1][0])
    p0, p1, p2, p3 = (track[k][1] for k in
                      (j-2 if j > 1 else len(track)-2, j-1, j, j+1 if j+1 < len(track) else 1))
    return [0.5*((2*b)+(-a+c)*t+(2*a-5*b+4*c-d)*t*t+(-a+3*b-3*c+d)*t*t*t)
            for a, b, c, d in zip(p0, p1, p2, p3)]


def verify_dataset(root, workspace):
    meta = json.loads((root/'capture.json').read_text())
    manifest, path = rows(root/'manifest.csv'), rows(root/'camera_path.csv')
    assert meta['status'] == 'complete' and meta['sequence'] == 'maku-gsplat'
    assert len(manifest) == len(path) == meta['view_count']
    assert meta['points'] > 100 and meta['hfov_degrees'] == 80
    assert meta['width'] == 320 and meta['height'] == 160
    # Reference positions resolved from the original XM/script at 22050 Hz.
    assert meta['segments'] == [
        dict(song_position=0xd00, start_sample=0, track_offset_seconds=160.5, speed=-3),
        dict(song_position=0xe00, start_sample=179390, track_offset_seconds=25.5, speed=2),
        dict(song_position=0xe20, start_sample=269085, track_offset_seconds=0, speed=2.5),
        dict(song_position=0xf00, start_sample=358780, track_offset_seconds=42.5, speed=-2),
        dict(song_position=0xf20, start_sample=448475, track_offset_seconds=55.5, speed=4),
    ]
    assert abs(meta['duration_seconds']-538170/22050) < 1e-10
    tracks = ase_tracks(workspace)
    groups = set()
    for i, (row, pose) in enumerate(zip(manifest, path)):
        time = float(pose['scene_time_seconds'])
        assert time == float(row['scene_time_seconds'])
        assert abs(time-(i*538170//len(path))/22050) < 1e-10
        segment = next(s for s in reversed(meta['segments']) if s['start_sample']/22050 <= time)
        expected_time = (time-segment['start_sample']/22050)*segment['speed']+segment['track_offset_seconds']
        assert abs(float(pose['track_time_seconds'])-expected_time) < 3e-5
        assert pose['group'] == row['group'] == f'maku_{segment["song_position"]:x}'
        groups.add(pose['group'])
        assert float(pose['hfov_degrees']) == 80 and float(pose['roll_radians']) == 0
        for name, keys in [('Camera01', ('px', 'py', 'pz')), ('Camera01.Target', ('tx', 'ty', 'tz'))]:
            expected = spline(tracks[name], expected_time)
            actual = list(map(lambda key: float(pose[key]), keys))
            assert math.dist(expected, actual) < 0.003, (i, name, expected, actual)
        read_png(root/row['file'], meta['width'], meta['height'])
    assert len(groups) == 5

    split_ids = []
    max_error = 0
    for split, prefix in [('train', root), ('validation', root/'validation')]:
        cameras, images, points = read_model(prefix/'sparse')
        expected = {int(row['image_id']) for row in manifest if row['split'] == split}
        assert set(images) == expected
        split_ids.append(expected)
        assert set(cameras) == {1}
        if split == 'train':
            assert len(points) == meta['points']
            assert all(len(track) >= 2 for _, track in points.values())
        for image_id, view in images.items():
            assert (prefix/'images'/view['name']).exists()
            source = path[image_id-1]
            center = [float(source[k]) for k in ('px', 'py', 'pz')]
            target = [float(source[k]) for k in ('tx', 'ty', 'tz')]
            forward = normalize([a-b for a, b in zip(target, center)])
            right = normalize(cross((0, 0, 1), forward))
            up = cross(forward, right)
            rotation, translation = view['rotation'], view['translation']
            exported_center = [-sum(rotation[r][c]*translation[r] for r in range(3)) for c in range(3)]
            assert math.dist(exported_center, [-center[0], *center[1:]]) < 1e-8
            width, height, fx, fy, cx, cy = cameras[view['camera']]
            assert (width, height) == (meta['width'], meta['height'])
            assert abs(fx-width/2/math.tan(math.radians(80)/2)) < 0.001
            assert fx == fy and cx == width/2 and cy == height/2
            for observation_id, (x, y, point_id) in enumerate(view['observations']):
                world, track = points[point_id]
                assert (image_id, observation_id) in track
                projected = [dot(axis, world)+t for axis, t in zip(rotation, translation)]
                assert 0.1 < projected[2] < 200
                error = math.hypot(cx+fx*projected[0]/projected[2]-x, cy+fy*projected[1]/projected[2]-y)
                assert error < 1e-7
                max_error = max(max_error, error)
                relative = [a-b for a, b in zip([-world[0], *world[1:]], center)]
                native_x = cx+fx*dot(relative, right)/dot(relative, forward)
                native_y = cy-fy*dot(relative, up)/dot(relative, forward)
                assert math.hypot(native_x-x, native_y-y) < 0.001
                assert 0 <= x < width and 0 <= y < height
            assert view['observations'], f'No initialization points for image {image_id}'
        for point_id, (_, track) in points.items():
            assert len({image for image, _ in track}) == len(track)
            for image, observation in track:
                assert images[image]['observations'][observation][2] == point_id
    assert not split_ids[0] & split_ids[1]
    assert len(split_ids[1]) == meta['validation_views']
    print(f'Validated {len(path)} Maku views, {meta["points"]} points; reprojection {max_error:.3g} px')


def main():
    executable, workspace = (Path(p).resolve() for p in sys.argv[1:3])
    with tempfile.TemporaryDirectory(prefix='maku-gsplat-') as temp:
        base = Path(temp)

        def run(output, *args, success=True):
            result = subprocess.run([str(executable), '--sequence', 'maku-gsplat', '--frames', '60',
                                     '--width', '320', '--height', '160', '--output', str(output), *args],
                                    cwd=workspace, capture_output=True, text=True)
            assert (result.returncode == 0) == success, result.stdout+result.stderr

        first, denser = base/'first', base/'denser'
        run(first)
        verify_dataset(first, workspace)
        # Same poses must render identical PNGs regardless of earlier captures,
        # sampling density, validation split, FPS and audio flags.
        run(denser, '--frames', '120', '--gsplat-validation-every', '0', '--fps', '29', '--sample-rate', '48000')
        first_rows, dense_rows = rows(first/'manifest.csv'), rows(denser/'manifest.csv')
        first_path, dense_path = rows(first/'camera_path.csv'), rows(denser/'camera_path.csv')
        assert first_path == dense_path[::2]
        for a, b in zip(first_rows, dense_rows[::2]):
            assert (first/a['file']).read_bytes() == (denser/b['file']).read_bytes()
        assert not (denser/'validation').exists()
        # FOV override must change image and intrinsics, preserving path/timing.
        wide = base/'wide'
        run(wide, '--gsplat-fov', '90')
        wide_path = rows(wide/'camera_path.csv')
        for a, b in zip(first_path, wide_path):
            assert {k: v for k, v in a.items() if k != 'hfov_degrees'} == {
                k: v for k, v in b.items() if k != 'hfov_degrees'}
        assert (first/'images/frame_000000.png').read_bytes() != (wide/'images/frame_000000.png').read_bytes()
        cameras, _, _ = read_model(wide/'sparse')
        assert abs(cameras[1][2]-160) < 0.001
        run(first, success=False)
        for arguments in [('--gsplat-fov', 'nan'), ('--gsplat-fov', '0'), ('--gsplat-fov', '121'),
                          ('--gsplat-fov', 'inf'), ('--gsplat-radius', '10'), ('--gsplat-time', '30'),
                          ('--gsplat-camera-path', str(first/'camera_path.csv')),
                          ('--gsplat-validation-every', '1'), ('--width', '4097'), ('--frames', '1'),
                          ('--until-song-position', '0x1000'), ('--post-roll-frames', '1'),
                          ('--sequence', 'maku', '--gsplat-fov', '80'),
                          ('--sequence', 'saari-gsplat', '--gsplat-fov', '80')]:
            output = base/'invalid'
            run(output, *arguments, success=False)
            assert not output.exists()


if __name__ == '__main__':
    main()
