# Forward Reference Capture Workflow

This workflow keeps the two sides separate:

- Java self-captures its own rendered frames.
- Python + FFmpeg extracts matching frames from the reference video.
- Python then compares both sets and produces a scene-oriented report.

## 1) Java self-capture

The desktop launcher still uses the legacy applet-style parameter model:

- every option must be passed as a `key value` pair
- even boolean switches must be written like `nosound 1` or `captureexit 1`

Example capture run:

```bat
run_forward_desktop.bat ^
  capture documentation\reference-capture\java ^
  captureintervalms 2000 ^
  capturestartms 0 ^
  capturestopms 120000 ^
  capturelimit 60 ^
  captureexit 1
```

Ready-made wrapper:

```bat
capture_forward_demo.bat
```

Generated output:

- `documentation/reference-capture/java/manifest.csv`
- `documentation/reference-capture/java/frames/*.png`

Manifest timing fields:

- `demo_time_ms` / `demo_time_seconds`: global elapsed time since the start of the captured run
- `scene_time_ms` / `scene_time_seconds`: local elapsed time inside the currently active scene

Important caveat for the frozen Java baseline:

- in the desktop Java capture path, `demo_time_ms` is capture-session wall-clock, not a sample-accurate demo master clock
- the frozen baseline was captured with `captureintervalms 4000`, so full-demo frames are sparse by design
- around scene starts, Java `scene_time_ms` comes from the legacy smoothed timer and can lead wall-clock by a noticeable amount

Practical consequence:

- do not infer a true constant cross-port delay from the baseline frame filenames alone
- do not align C++ full exports to the Java baseline by `capture_index` alone
- prefer `scene` + `next_script_time_hex`, then refine with `scene_time_ms` and visual matching

### Java capture parameters

- `capture <dir>`: enables capture and writes output under this directory
- relative `capture` paths are resolved from the repository root when launched through `run_forward_desktop.bat`
- `captureintervalms <ms>`: recommended stable cadence based on demo time
- `captureevery <n>`: alternate cadence based on render frames
- `capturestartms <ms>`: ignore earlier frames
- `capturestopms <ms>`: stop capturing after this demo timestamp
- `capturelimit <n>`: stop after `n` saved frames
- `captureexit 1`: close the desktop app automatically when capture is complete

Recommendation:

- prefer `captureintervalms` for reference alignment
- use `captureevery` only for local exploratory runs, because render-frame cadence depends on machine speed
- the ready-made wrapper defaults to one capture every `2000 ms`, which is about 10x fewer images than the original `200 ms` setup
- keep audio enabled for reference capture; `nosound 1` is useful for smoke tests but can distort scene timing enough to skip whole sections of the demo
- for scene-fidelity work, prefer denser ad hoc captures than the frozen `4000 ms` baseline cadence

## 2) Extract reference frames from the YouTube capture

The Python tool reads the Java manifest and asks FFmpeg for the same timestamps.

Example:

```bat
python tools\forward_reference_compare.py extract-video ^
  --video "original\youtube\Komplex - Forward (Java demo, 1998) [QkJK_voQBis].mp4" ^
  --java-manifest documentation\reference-capture\java\manifest.csv ^
  --output-dir documentation\reference-capture\reference ^
  --video-offset-ms 0 ^
  --video-filter "scale=512:256:flags=lanczos"
```

Ready-made wrapper:

```bat
capture_reference_video.bat
```

If the source video includes borders or a window frame, replace the filter with an explicit crop + scale chain, for example:

```text
crop=640:320:40:30,scale=512:256:flags=lanczos
```

Generated output:

- `documentation/reference-capture/reference/reference_manifest.csv`
- `documentation/reference-capture/reference/frames/*.png`

## 3) Compare Java vs reference

Example:

```bat
python tools\forward_reference_compare.py compare ^
  --java-manifest documentation\reference-capture\java\manifest.csv ^
  --reference-manifest documentation\reference-capture\reference\reference_manifest.csv ^
  --output-dir documentation\reference-capture\compare ^
  --write-visuals ^
  --visual-limit 24
```

Generated output:

- `frame_metrics.csv`: per-frame metrics
- `summary.md`: per-scene priorities and suggested code focus
- `visuals/*_compare.png`: side-by-side Java/reference images for the worst frames
- `visuals/*_diff.png`: boosted absolute-difference images for the worst frames

## 4) How to read the comparison report

The report is intended as triage, not as an automatic patch generator.

Heuristics used:

- large full-frame mismatch across most scenes:
  verify `--video-offset-ms` and crop/scale first, then inspect shared framebuffer/timing code
- scene-local mismatch with mostly matching composition:
  inspect that scene's renderer/effect class
- strong global brightness mismatch:
  inspect fade, blend, palette, or post-processing paths
- band-shaped mismatch:
  inspect scroll, wrap, and copy-window logic

Primary Java scene files:

- `mute95`: `kmjjkmk.java`
- `domina`: `kajakka.java`
- `saari`: `maajmka.java`
- `kukot`: `kajjkka.java`
- `maku`: `kmjjmka.java`
- `watercube`: `kmajmka.java`
- `feta`: `kmaamka.java`
- `uppol`: `mmaakmk.java`

Shared fallback files when the mismatch is global:

- `forward.java`
- `kaaakka.java`
- `mmaamma.java`
- `kaajmma.java`
