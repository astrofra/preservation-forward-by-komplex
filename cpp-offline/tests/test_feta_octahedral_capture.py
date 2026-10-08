"""Octahedral camera coverage, tangent perturbation, and replay checks."""
import math
from pathlib import Path
import subprocess
import sys
import tempfile

from test_feta_capture import rows, verify_dataset
from test_saari_capture import dot, normalize


def encode(direction):
    # Independent inverse mapping of exported positions to the source UV grid.
    x, y, z = direction
    length = abs(x) + abs(y) + abs(z)
    x, y = x/length, y/length
    if z < 0:
        x, y = (1-abs(y))*(-1 if x < 0 else 1), (1-abs(x))*(-1 if y < 0 else 1)
    return x, y


def main():
    executable, workspace = (Path(p).resolve() for p in sys.argv[1:3])
    with tempfile.TemporaryDirectory(prefix='feta-octahedral-') as temp:
        base = Path(temp)

        def run(name, *args, success=True):
            output = base/name
            result = subprocess.run([str(executable), '--sequence', 'feta-gsplat',
                '--gsplat-sampling', 'octahedral', '--frames', '64', '--width', '384',
                '--height', '288', '--output', str(output), *args], cwd=workspace,
                capture_output=True, text=True)
            assert (result.returncode == 0) == success, result.stdout + result.stderr
            if not success:
                assert not output.exists()
            return output

        plain = run('plain')
        meta, positions = verify_dataset(plain)
        assert meta['path_kind'] == 'octahedral' and meta['octahedral_grid_side'] == 8
        center = meta['orbit_center_native']
        directions = [normalize([a-b for a, b in zip(p, center)]) for p in positions]
        radius = meta['radius_start']
        assert meta['radius_max'] - meta['radius_min'] < 1e-5
        for i, direction in enumerate(directions):
            expected_uv = (2*((i % 8)+0.5)/8-1, 2*((i // 8)+0.5)/8-1)
            assert math.dist(encode(direction), expected_uv) < 1e-6
        assert len({tuple(round(x, 5) for x in d) for d in directions}) == 64
        assert sum(d[2] > 0.1 for d in directions) >= 16
        assert sum(d[2] < -0.1 for d in directions) >= 16
        assert all(abs(sum(d[axis] for d in directions)) < 1e-5 for axis in range(3))

        offset = run('offset', '--gsplat-lateral-offset', '0.15')
        offset_meta, offset_positions = verify_dataset(offset)
        assert offset_meta['path_kind'] == 'octahedral_offset'
        assert abs(offset_meta['lateral_offset_applied']-0.15) < 1e-7
        assert offset_meta['radius_max'] - offset_meta['radius_min'] < 1e-5
        angle = math.atan(0.15/radius)
        expected_chord = 2*radius*math.sin(angle/2)
        for p, shifted, direction in zip(positions, offset_positions, directions):
            actual = normalize([a-b for a, b in zip(shifted, center)])
            assert abs(math.acos(max(-1, min(1, dot(direction, actual))))-angle) < 1e-6
            assert abs(math.dist(p, shifted)-expected_chord) < 1e-5

        target = run('target', '--gsplat-target-offset', '0.15')
        target_meta, target_positions = verify_dataset(target)
        assert target_meta['path_kind'] == 'octahedral_target'
        assert target_positions == positions
        assert abs(target_meta['target_offset_applied']-0.15) < 1e-7
        offsets = []
        for direction, pose in zip(directions, rows(target/'camera_path.csv')):
            delta = [float(pose[k])-c for k,c in zip(('tx','ty','tz'),center)]
            offsets.append(math.sqrt(dot(delta,delta)))
            assert abs(dot(delta,direction)) < 1e-6
        assert min(offsets) > 0 and max(offsets) < 0.15+1e-6
        # Uniform disk area: the median radius should be near R/sqrt(2).
        assert abs(sorted(offsets)[32]-0.15/math.sqrt(2)) < 0.003
        assert all((plain/a['file']).read_bytes() != (target/b['file']).read_bytes()
                   for a,b in zip(rows(plain/'manifest.csv'), rows(target/'manifest.csv')))

        # Exported positions and targets are authoritative: no double perturbation.
        csv_lines = (target/'camera_path.csv').read_text().splitlines()
        reverse = base/'reverse.csv'
        reverse.write_text('\n'.join([csv_lines[0], *reversed(csv_lines[1:])])+'\n')
        replay = run('replay', '--gsplat-camera-path', str(reverse), '--frames', '13',
                     '--gsplat-lateral-offset', '0.9', '--gsplat-target-offset', '0.9', '--gsplat-end-radius', '9')
        replay_meta, _ = verify_dataset(replay)
        assert replay_meta['path_kind'] == 'csv' and replay_meta['lateral_offset_applied'] == 0
        assert replay_meta['target_offset_applied'] == 0
        for old, new in zip(reversed(rows(target/'manifest.csv')), rows(replay/'manifest.csv')):
            assert (target/old['file']).read_bytes() == (replay/new['file']).read_bytes()

        # An odd grid contains the exact north pole and still needs a valid camera.
        odd = run('odd', '--frames', '25')
        odd_meta, odd_positions = verify_dataset(odd)
        assert odd_meta['octahedral_grid_side'] == 5
        assert math.dist(odd_positions[12][:2], center[:2]) < 1e-7
        assert odd_positions[12][2] > center[2]
        odd_offset = run('odd-offset', '--frames', '25', '--gsplat-lateral-offset', '0.15')
        verify_dataset(odd_offset)
        odd_target = run('odd-target', '--frames', '25', '--gsplat-target-offset', '0.15')
        verify_dataset(odd_target)

        for i, args in enumerate([
            ('--frames', '300'), ('--frames', '63'), ('--gsplat-end-radius', '9'),
            ('--gsplat-lateral-offset', '-0.1'), ('--gsplat-lateral-offset', 'nan'),
            ('--gsplat-lateral-offset', '1.1'), ('--gsplat-sampling', 'unknown'),
            ('--gsplat-target-offset', 'nan'), ('--gsplat-target-offset', '-0.1'),
            ('--gsplat-target-offset', '1.1'),
            ('--gsplat-sampling', 'orbit', '--gsplat-lateral-offset', '0.15'),
            ('--gsplat-sampling', 'orbit', '--gsplat-target-offset', '0.15'),
            ('--sequence', 'maku-gsplat'), ('--sequence', 'saari-gsplat')]):
            run(f'invalid-{i}', *args, success=False)
        print('Octahedral mapping, spherical radius, origin/target offsets, polar cameras and exact replay passed.')


if __name__ == '__main__':
    main()
