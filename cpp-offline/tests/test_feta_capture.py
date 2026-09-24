"""Feta export integration checks; Python stdlib is only a test dependency."""
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


def verify_dataset(root):
    meta = json.loads((root/'capture.json').read_text())
    manifest, path = rows(root/'manifest.csv'), rows(root/'camera_path.csv')
    particles = rows(root/'particles.csv')
    assert meta['status'] == 'complete' and meta['sequence'] == 'feta-gsplat'
    assert len(manifest) == len(path) == meta['view_count']
    assert meta['fetus_points'] > 100 and meta['particle_points'] > 0
    assert meta['fetus_points'] + meta['particle_points'] == meta['points']
    assert len(particles) == meta['particle_count'] == 300
    assert {int(p['particle_id']) for p in particles} == set(range(300))
    orbit_center = meta['orbit_center_native']
    assert meta['radius_min'] >= meta['fetus_radius'] + 0.5 - 1e-4
    # The sprite's native 20/depth pixel size defines a fixed world-space side.
    assert abs(meta['particle_world_size']-20/(256/math.tan(1.9/2))) < 1e-7
    particle_points = {}
    for particle in particles:
        center = list(map(float, (particle[k] for k in ('x', 'y', 'z'))))
        assert math.dist(center, orbit_center) < meta['enclosing_radius']
        assert float(particle['world_size']) == meta['particle_world_size']
        particle_points[int(particle['point3D_id'])] = [-center[0], *center[1:]]
    positions = []
    for row, pose in zip(manifest, path):
        assert float(row['scene_time_seconds']) == meta['scene_time_seconds']
        position = list(map(float, (pose[k] for k in ('px', 'py', 'pz'))))
        positions.append(position)
        read_png(root/row['file'], meta['width'], meta['height'])
    radii = [math.dist(p, orbit_center) for p in positions]
    for key, value in [('radius_start', radii[0]), ('radius_end', radii[-1]),
                       ('radius_min', min(radii)), ('radius_max', max(radii))]:
        assert abs(meta[key]-value) < 1e-8
    if meta['path_kind'] == 'progressive':
        assert radii[0] >= meta['enclosing_radius'] + 0.5 - 1e-4
        assert all(b <= a+1e-4 for a, b in zip(radii, radii[1:]))
        assert all([float(p[k]) for k in ('tx', 'ty', 'tz')] == orbit_center for p in path)

    splits, max_error = [], 0
    for split, prefix in [('train', root), ('validation', root/'validation')]:
        if not prefix.exists():
            continue
        cameras, images, points = read_model(prefix/'sparse')
        expected = {int(row['image_id']) for row in manifest if row['split'] == split}
        assert set(images) == expected
        splits.append(expected)
        if split == 'train':
            assert len(points) == meta['points']
            assert all(len(track) >= 2 for _, track in points.values())
            assert len(set(points) & set(particle_points)) == meta['particle_points']
        for point_id, (world, track) in points.items():
            # No fictitious finite background geometry in the seed model.
            assert math.dist([-world[0], *world[1:]], orbit_center) < meta['enclosing_radius']
            if point_id in particle_points:
                assert math.dist(world, particle_points[point_id]) < 1e-10
            else:
                assert point_id < min(particle_points)
            assert len({image for image, _ in track}) == len(track)
            for image, observation in track:
                assert images[image]['observations'][observation][2] == point_id
        for image_id, view in images.items():
            assert (prefix/'images'/view['name']).exists()
            source = path[image_id-1]
            native_center = positions[image_id-1]
            target = [float(source[k]) for k in ('tx', 'ty', 'tz')]
            forward = normalize([a-b for a, b in zip(target, native_center)])
            right = cross([0, 0, 1], forward)
            right = normalize(right) if dot(right, right) > 1e-12 else [1, 0, 0]
            up = cross(forward, right)
            rotation, translation = view['rotation'], view['translation']
            center = [-sum(rotation[r][c]*translation[r] for r in range(3)) for c in range(3)]
            assert math.dist(center, [-native_center[0], *native_center[1:]]) < 1e-8
            width, height, fx, fy, cx, cy = cameras[view['camera']]
            assert (width, height) == (meta['width'], meta['height'])
            assert abs(fx-width/2/math.tan(math.radians(float(source['hfov_degrees']))/2)) < 0.001
            assert fx == fy and cx == width/2 and cy == height/2
            for observation_id, (x, y, point_id) in enumerate(view['observations']):
                world, track = points[point_id]
                assert (image_id, observation_id) in track
                projected = [dot(axis, world)+t for axis, t in zip(rotation, translation)]
                assert projected[2] > 0
                error = math.hypot(cx+fx*projected[0]/projected[2]-x,
                                   cy+fy*projected[1]/projected[2]-y)
                assert error < 1e-7
                max_error = max(max_error, error)
                relative = [a-b for a, b in zip([-world[0], *world[1:]], native_center)]
                native_x = cx+fx*dot(relative, right)/dot(relative, forward)
                native_y = cy-fy*dot(relative, up)/dot(relative, forward)
                assert math.hypot(native_x-x, native_y-y) < 0.001
                assert 0 <= x < width and 0 <= y < height
    assert len(splits) == 1 or not splits[0] & splits[1]
    print(f'Validated {len(path)} PNGs, {meta["points"]} points; reprojection error {max_error:.3g} px')
    return meta, positions


def main():
    executable, workspace = (Path(p).resolve() for p in sys.argv[1:3])
    with tempfile.TemporaryDirectory(prefix='feta-gsplat-') as temp:
        base = Path(temp)

        def run(output, *args, success=True):
            result = subprocess.run([str(executable), '--sequence', 'feta-gsplat', '--frames', '48',
                                     '--width', '384', '--height', '288', '--output', str(output), *args],
                                    cwd=workspace, capture_output=True, text=True)
            assert (result.returncode == 0) == success, result.stdout + result.stderr
            return result

        first = base/'first'
        run(first)
        meta, positions = verify_dataset(first)
        assert meta['scene_time_seconds'] == 0 and meta['validation_views'] == 4
        assert meta['particle_points'] > 100
        center = meta['orbit_center_native']
        directions = [normalize([a-b for a, b in zip(p, center)]) for p in positions]
        assert meta['path_kind'] == 'progressive'
        assert meta['radius_start']/meta['radius_end'] > 2
        # Close views must cover all sides too, not just one end of a spherical spiral.
        for third in range(3):
            section = directions[third*len(directions)//3:(third+1)*len(directions)//3]
            assert len({tuple(p > 0 for p in direction) for direction in section}) == 8
            assert min(p[2] for p in section) < -0.95 and max(p[2] for p in section) > 0.95
        radii = [math.dist(p, center) for p in positions]
        assert all(b < a for a, b in zip(radii, radii[1:]))
        assert max(a-b for a, b in zip(radii, radii[1:])) < (radii[0]-radii[-1])*0.04

        # Bounds come from actual mesh vertices, independently of seed sampling.
        mesh = (workspace/'original/forward/meshes/fetus.igu').read_text()
        vertex_block = re.search(r'Vertices:\s*(\d+)', mesh)
        assert vertex_block
        vertices = [tuple(float(c)*0.09 for c in v) for v in
                    re.findall(r'X:\s*([-+\d.eE]+), Y:\s*([-+\d.eE]+), Z:\s*([-+\d.eE]+)',
                               mesh[vertex_block.end():])[:int(vertex_block[1])]]
        expected_center = [(min(v[i] for v in vertices)+max(v[i] for v in vertices))/2 for i in range(3)]
        assert math.dist(center, expected_center) < 1e-6
        assert abs(meta['fetus_radius']-max(math.dist(v, center) for v in vertices)) < 1e-6
        for pose, position in zip(rows(first/'camera_path.csv'), positions):
            forward = normalize([a-b for a, b in zip(center, position)])
            right = normalize(cross([0, 0, 1], forward))
            up = cross(forward, right)
            half_h = math.tan(math.radians(float(pose['hfov_degrees']))/2)
            half_v = half_h*meta['height']/meta['width']
            for vertex in vertices:
                relative = [a-b for a, b in zip(vertex, position)]
                depth = dot(relative, forward)
                assert depth > 0.1
                assert abs(dot(relative, right)) < half_h*depth
                assert abs(dot(relative, up)) < half_v*depth
        assert all(abs(float(p[k])) <= 5 for p in rows(first/'particles.csv') for k in ('x', 'y', 'z'))

        # Reordering views must not change images: no animation or temporal feedback.
        csv_lines = (first/'camera_path.csv').read_text().splitlines()
        reverse_csv = base/'reverse.csv'
        reverse_csv.write_text('\n'.join([csv_lines[0], *reversed(csv_lines[1:])])+'\n')
        replay = base/'replay'
        run(replay, '--gsplat-camera-path', str(reverse_csv), '--fps', '29')
        original_rows = rows(first/'manifest.csv')
        for original, replayed in zip(reversed(original_rows), rows(replay/'manifest.csv')):
            assert (first/original['file']).read_bytes() == (replay/replayed['file']).read_bytes()
        assert (first/'particles.csv').read_bytes() == (replay/'particles.csv').read_bytes()
        replay_meta = json.loads((replay/'capture.json').read_text())
        assert replay_meta['path_kind'] == 'csv' and replay_meta['radius_start'] < replay_meta['radius_end']

        duplicate_csv = base/'duplicate.csv'
        duplicate_csv.write_text('\n'.join([csv_lines[0]]+[csv_lines[1]]*12)+'\n')
        duplicate = base/'duplicate'
        run(duplicate, '--gsplat-camera-path', str(duplicate_csv), '--gsplat-validation-every', '0')
        images = list((duplicate/'images').glob('*.png'))
        assert len(images) == 12 and len({p.read_bytes() for p in images}) == 1
        assert not (duplicate/'validation').exists()

        # A different frozen time moves particles once, independently of camera order.
        shifted = base/'shifted'
        run(shifted, '--gsplat-camera-path', str(first/'camera_path.csv'), '--gsplat-time', '1')
        verify_dataset(shifted)
        for a, b in zip(rows(first/'particles.csv'), rows(shifted/'particles.csv')):
            x, y, z = (float(a[k]) for k in ('x', 'y', 'z'))
            expected = [x*math.cos(-0.5)-y*math.sin(-0.5), x*math.sin(-0.5)+y*math.cos(-0.5), z]
            assert math.dist(expected, [float(b[k]) for k in ('x', 'y', 'z')]) < 1e-6
        assert (first/original_rows[0]['file']).read_bytes() != (shifted/original_rows[0]['file']).read_bytes()

        # Exact poles exercise the alternate up axis; varying FOVs exercise camera IDs.
        pole_csv = base/'poles.csv'
        radius = meta['radius_start']
        pole_csv.write_text('\n'.join([csv_lines[0]]+
            [f'0,0,{sign*radius},0,0,0,{fov},custom'
             for sign in (-1, 1) for fov in (80, 90) for _ in range(3)])+'\n')
        poles = base/'poles'
        run(poles, '--gsplat-camera-path', str(pole_csv), '--gsplat-validation-every', '0')
        verify_dataset(poles)
        assert len(read_model(poles/'sparse')[0]) == 2

        override = base/'override'
        run(override, '--gsplat-radius', '20', '--gsplat-end-radius', '9')
        override_meta, _ = verify_dataset(override)
        assert abs(override_meta['radius_start']-20) < 1e-4
        assert abs(override_meta['radius_end']-9) < 1e-4

        # Legacy origin-centered constant-radius CSVs still load unchanged.
        legacy_csv = base/'legacy.csv'
        legacy_csv.write_text('\n'.join([csv_lines[0]]+
            [f'{16*math.cos(i*math.pi/6)},{16*math.sin(i*math.pi/6)},0,0,0,0,80,sphere'
             for i in range(12)])+'\n')
        run(base/'legacy', '--gsplat-camera-path', str(legacy_csv))

        run(first, success=False)
        for i, args in enumerate([('--gsplat-time', 'nan'), ('--gsplat-radius', '1'),
                                  ('--gsplat-radius', 'inf'), ('--gsplat-fov', 'nan'),
                                  ('--gsplat-end-radius', 'nan'), ('--gsplat-end-radius', '1'),
                                  ('--gsplat-end-radius', '30'),
                                  ('--sequence', 'saari-gsplat', '--gsplat-end-radius', '9'),
                                  ('--gsplat-fov', '121'), ('--gsplat-validation-every', '1'),
                                  ('--width', '4097'), ('--gsplat-height-fraction', '0.25'),
                                  ('--until-song-position', '0x100')]):
            output = base/f'invalid-{i}'
            run(output, *args, success=False)
            assert not output.exists()
        invalid_csv = base/'invalid.csv'
        for i, row in enumerate(['0,0,0,0,0,0,80,custom', '0,0,1,0,0,0,80,custom',
                                 f'0,0,{radius},0,0,{radius},80,custom']):
            invalid_csv.write_text('\n'.join([csv_lines[0]]+[row]*12)+'\n')
            output = base/f'invalid-path-{i}'
            run(output, '--gsplat-camera-path', str(invalid_csv), success=False)
            assert not output.exists()
        print('Progressive approach, close-view coverage, mesh framing, frozen replay and invalid-input checks passed.')


if __name__ == '__main__':
    main()
