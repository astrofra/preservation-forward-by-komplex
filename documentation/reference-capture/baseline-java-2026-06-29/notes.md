# Java Baseline Capture Notes

## Identity

- Baseline type: frozen Java behavioral reference
- Baseline date: June 29, 2026
- Git commit: `0fffbaf55f2b86155e1ab53d486cf1ec060dc9dc`
- Source capture directory at freeze time: `documentation/reference-capture/java/`

## Captured Artifacts

- `manifest.csv`
- `frames/`
- Frame count: `76`
- Demo time range: `341 ms` to `300002 ms`
- Native frame size: `512x256`

## Scene Coverage

- bootstrap/no-scene row: `1`
- `mute95`: `19`
- `domina`: `5`
- `saari`: `15`
- `kukot`: `12`
- `maku`: `6`
- `watercube`: `6`
- `feta`: `6`
- `uppol`: `6`

## Capture Provenance

The observed manifest timing matches the default wrapper settings in `capture_forward_demo.bat`:

```bat
capture_forward_demo.bat
```

Default parameters in that wrapper:

- `CAPTURE_DIR=documentation\reference-capture\java`
- `CAPTURE_INTERVAL_MS=4000`
- `CAPTURE_START_MS=0`
- `CAPTURE_STOP_MS=300000`
- `CAPTURE_LIMIT=250`
- `ENABLE_CAPTURE_EXIT=1`
- `ENABLE_NOSOUND=0`

Notes:

- The manifest is consistent with `captureintervalms 4000`.
- The manifest is consistent with `capturestopms 300000`.
- The run terminated before `capturelimit 250`, so `capturestopms` appears to be the effective stop condition.
- Audio is expected to have been enabled because the wrapper default is `ENABLE_NOSOUND=0`, but the manifest alone does not prove that no local override was used.
- `demo_time_ms` in this manifest is capture-session wall-clock (`System.currentTimeMillis() - sessionStartMs`), not the Java demo's internal sample-accurate master clock.
- Because the capture cadence is `4000 ms`, the frame filenames can suggest a constant `4 s` phase difference when compared naively against dense C++ exports.
- `scene_time_ms` is the better scene-local clue, but it still comes from the legacy smoothed timer and is not a perfect replacement for audio/song-position alignment.

## Toolchain Context

- Local `javac -version` observed during baseline normalization: `17.0.15`

## Alignment and Comparison Notes

- The first manifest row is a bootstrap frame with no active scene:
  - `capture_index=0`
  - `demo_time_ms=341`
  - `scene=""`
  - `next_script_time_hex=0x0`
- Automated comparisons should ignore or special-case that first row.
- Use `manifest.csv` as the primary alignment source.
- Recommended matching key order:
  1. `scene`
  2. nearest `demo_time_ms` within tolerance
  3. fallback to nearest `scene_time_ms`
  4. verify `next_script_time_hex`
- For dense C++ parity work, also verify whether the chosen Java frame is simply the nearest `4000 ms` capture sample rather than the exact behavioral match.

## Limits of This Baseline

- This full-timeline capture is good for whole-demo drift, scene order, and large visual regressions.
- It is relatively sparse for subtle scene-local regressions because the cadence is approximately one frame every `4000 ms`.
- For subsystem work such as scene cleanup or C++ fidelity debugging, add denser scene-window captures alongside this frozen baseline.
