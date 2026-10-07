"""Maku overhead grid and shared model checks; stdlib-only test dependency."""
import json
import math
from pathlib import Path
import subprocess
import sys
import tempfile

from test_maku_capture import ase_tracks, rows, spline
from test_saari_capture import cross, dot, normalize, read_model, read_png


def verify_dataset(root, workspace):
    meta = json.loads((root/'capture.json').read_text())
    grid = meta['grid']
    path, manifest = rows(root/'camera_path.csv'), rows(root/'manifest.csv')
    assert meta['status'] == 'complete' and grid['enabled']
    assert len(path) == len(manifest) == meta['view_count']
    path_views = meta['views_per_pass'] * (2 if meta['height_offset'] else 1)
    assert len(path) == path_views + grid['view_count']
    assert [p['name'] for p in meta['passes']] == (['original', 'raised', 'grid'] if meta['height_offset'] else ['original', 'grid'])
    assert grid['view_count'] == grid['columns']*grid['rows']*17
    assert grid['views_per_station'] == 17 and grid['azimuth_step_degrees'] == 45
    assert grid['downward_degrees'] == [30, 60, 90]
    assert abs(grid['z']-meta['terrain_z_max']-grid['clearance']) < 1e-8

    # Reference bounds from the source ASE, only on script intervals actually played.
    rate = meta['timeline_sample_rate']
    end_sample = round(meta['duration_seconds']*rate)
    samples = set(range(0, end_sample, rate//grid['bounds_sample_rate'])) | {end_sample-1}
    for segment in meta['segments']:
        samples.add(segment['start_sample'])
        if segment['start_sample']:
            samples.add(segment['start_sample']-1)
    track = ase_tracks(workspace)['Camera01']
    reference = []
    for sample in samples:
        segment = next(s for s in reversed(meta['segments']) if s['start_sample'] <= sample)
        seconds = (sample-segment['start_sample'])/rate*segment['speed']+segment['track_offset_seconds']
        reference.append(spline(track, seconds))
    for axis in range(3):
        assert abs(min(p[axis] for p in reference)-grid['camera_min_native'][axis]) < 0.005
        assert abs(max(p[axis] for p in reference)-grid['camera_max_native'][axis]) < 0.005
    for axis, count, spacing in [(0, grid['columns'], grid['spacing_x']), (1, grid['rows'], grid['spacing_y'])]:
        low, high = grid['camera_min_native'][axis], grid['camera_max_native'][axis]
        assert abs((grid['min_xy'][axis]+grid['max_xy'][axis])-(low+high)) < 1e-8
        assert abs(grid['max_xy'][axis]-grid['min_xy'][axis]-(high-low)*grid['scale']) < 1e-8
        assert abs(spacing*(count-1)-(high-low)*grid['scale']) < 1e-8
        assert 0 < spacing <= grid['maximum_spacing']

    stations = {}
    for i, (pose, entry) in enumerate(zip(path, manifest)):
        for key in ('pass', 'height_offset', 'grid_x', 'grid_y', 'azimuth_degrees', 'downward_degrees'):
            assert pose[key] == entry[key]
        read_png(root/entry['file'], meta['width'], meta['height'])
        local_index = i-path_views if i >= path_views else i % meta['views_per_pass']
        held_out = meta['validation_every'] > 0 and (local_index+1) % meta['validation_every'] == 0
        assert entry['split'] == ('validation' if held_out else 'train')
        if i < path_views:
            assert all(pose[k] == '' for k in ('grid_x', 'grid_y', 'azimuth_degrees', 'downward_degrees'))
            continue
        assert pose['pass'] == 'grid' and pose['group'] == 'maku_grid'
        assert pose['track_time_seconds'] == pose['height_offset'] == ''
        assert float(pose['scene_time_seconds']) == 0
        column, row = int(pose['grid_x']), int(pose['grid_y'])
        assert 0 <= column < grid['columns'] and 0 <= row < grid['rows']
        p = [float(pose[k]) for k in ('px', 'py', 'pz')]
        expected = [grid['min_xy'][0]+column*grid['spacing_x'], grid['min_xy'][1]+row*grid['spacing_y'], grid['z']]
        assert math.dist(p, expected) < 1e-4
        assert p[2] > meta['terrain_z_max']
        az, down = float(pose['azimuth_degrees']), float(pose['downward_degrees'])
        stations.setdefault((column, row), []).append((az, down))
        forward = normalize([float(pose[k])-v for k, v in zip(('tx', 'ty', 'tz'), p)])
        expected = [math.cos(math.radians(down))*math.cos(math.radians(az)),
                    math.cos(math.radians(down))*math.sin(math.radians(az)), -math.sin(math.radians(down))]
        assert math.dist(forward, expected) < 1e-6
    expected_angles = {(az, down) for az in range(0, 360, 45) for down in (30, 60)} | {(0, 90)}
    assert len(stations) == grid['columns']*grid['rows']
    assert all(len(angles) == 17 and set(angles) == expected_angles for angles in stations.values())

    splits, max_error = [], 0
    for split, prefix in [('train', root), ('validation', root/'validation')]:
        if not prefix.exists():
            continue
        cameras, images, points = read_model(prefix/'sparse')
        assert set(cameras) == {1}
        expected = {int(r['image_id']) for r in manifest if r['split'] == split}
        assert set(images) == expected
        splits.append(expected)
        if split == 'train':
            assert len(points) == meta['points']
            assert all(len(track) >= 2 for _, track in points.values())
            grid_points = sum(any(i > path_views for i, _ in track) for _, track in points.values())
            shared = sum(any(i > path_views for i, _ in track) and any(i <= path_views for i, _ in track)
                         for _, track in points.values())
            empty = sum(not v['observations'] for i, v in images.items() if i > path_views)
            assert grid_points == grid['points'] and grid_points > 0
            assert shared == grid['points_shared_with_paths'] and shared > 0
            assert empty == grid['training_views_without_points']
        for image_id, view in images.items():
            pose = path[image_id-1]
            p = [float(pose[k]) for k in ('px', 'py', 'pz')]
            rotation, translation = view['rotation'], view['translation']
            center = [-sum(rotation[r][c]*translation[r] for r in range(3)) for c in range(3)]
            assert math.dist(center, [-p[0], *p[1:]]) < 1e-8
            # Use the original basis for the raised pass to retain float rounding.
            basis = path[(image_id-1) % meta['views_per_pass']] if pose['pass'] == 'raised' else pose
            forward = normalize([float(basis[t])-float(basis[p]) for t, p in zip(('tx','ty','tz'), ('px','py','pz'))])
            right = cross([0, 0, 1], forward)
            right = normalize(right) if dot(right, right) > 1e-12 else [1, 0, 0]
            up = cross(forward, right)
            width, height, fx, fy, cx, cy = cameras[view['camera']]
            assert (width, height) == (meta['width'], meta['height'])
            assert fx == fy and abs(fx-width/2/math.tan(math.radians(meta['hfov_degrees'])/2)) < 0.001
            for obs, (x, y, point_id) in enumerate(view['observations']):
                world, track = points[point_id]
                assert (image_id, obs) in track
                camera = [dot(axis, world)+t for axis, t in zip(rotation, translation)]
                assert 0.1 < camera[2] < 200
                error = math.hypot(cx+fx*camera[0]/camera[2]-x, cy+fy*camera[1]/camera[2]-y)
                assert error < 1e-7
                max_error = max(max_error, error)
                rel = [a-b for a, b in zip([-world[0], *world[1:]], p)]
                assert math.hypot(cx+fx*dot(rel, right)/dot(rel, forward)-x,
                                  cy-fy*dot(rel, up)/dot(rel, forward)-y) < 0.001
                assert 0 <= x < width and 0 <= y < height
        for point_id, (_, track) in points.items():
            assert len({i for i, _ in track}) == len(track)
            assert all(images[i]['observations'][o][2] == point_id for i, o in track)
    assert len(splits) == 1 or not splits[0] & splits[1]
    print(f'Validated {len(path)} views, {len(stations)} stations, shared points {grid["points_shared_with_paths"]}; error {max_error:.3g} px')


def main():
    executable, workspace = (Path(p).resolve() for p in sys.argv[1:3])
    with tempfile.TemporaryDirectory(prefix='maku-grid-') as temp:
        base = Path(temp)

        def run(output, *args, success=True):
            result = subprocess.run([str(executable), '--sequence', 'maku-gsplat', '--frames', '12',
                                     '--width', '320', '--height', '160', '--gsplat-grid-spacing', '400',
                                     '--output', str(output), *args], cwd=workspace, capture_output=True, text=True)
            assert (result.returncode == 0) == success, result.stdout+result.stderr

        first = base/'first'
        run(first)
        verify_dataset(first, workspace)
        meta = json.loads((first/'capture.json').read_text())
        assert meta['grid']['scale'] == 1.5
        assert abs(meta['grid']['clearance']-(meta['terrain_z_max']-meta['terrain_z_min'])/8) < 1e-8
        path, manifest = rows(first/'camera_path.csv'), rows(first/'manifest.csv')
        legacy = base/'legacy'
        run(legacy, '--gsplat-grid-scale', '0')
        assert rows(legacy/'camera_path.csv') == path[:24]
        for a, b in zip(manifest, rows(legacy/'manifest.csv')):
            assert (first/a['file']).read_bytes() == (legacy/b['file']).read_bytes()
        # Grid bounds, poses and pixels do not depend on temporal path sampling,
        # the raised pass or validation/FPS settings.
        denser = base/'denser'
        run(denser, '--frames', '24', '--gsplat-height-fraction', '0', '--gsplat-validation-every', '0', '--fps', '29')
        dense_path, dense_manifest = rows(denser/'camera_path.csv'), rows(denser/'manifest.csv')
        assert path[24:] == dense_path[24:]
        for a, b in zip(manifest[24:], dense_manifest[24:]):
            assert (first/a['file']).read_bytes() == (denser/b['file']).read_bytes()
        other = base/'other'
        run(other, '--gsplat-grid-scale', '1', '--gsplat-grid-clearance', '20')
        verify_dataset(other, workspace)
        other_meta = json.loads((other/'capture.json').read_text())
        assert other_meta['grid']['scale'] == 1 and other_meta['grid']['clearance'] == 20
        run(first, success=False)
        for args in [('--gsplat-grid-scale', 'nan'), ('--gsplat-grid-scale', '.5'), ('--gsplat-grid-scale', '3.1'),
                     ('--gsplat-grid-spacing', '0'), ('--gsplat-grid-clearance', '-1'), ('--gsplat-grid-clearance', 'inf'),
                     ('--gsplat-grid-scale', '3', '--gsplat-grid-spacing', '10'),
                     ('--sequence', 'feta-gsplat', '--gsplat-grid-scale', '1.5'),
                     ('--sequence', 'saari-gsplat', '--gsplat-grid-clearance', '20')]:
            run(base/'invalid', *args, success=False)
            assert not (base/'invalid').exists()


if __name__ == '__main__':
    main()
