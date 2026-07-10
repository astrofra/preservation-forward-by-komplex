# Forward C++11 Offline Scaffold

This directory contains the first usable milestone of the `documentation/forward-cpp11-subset-offline-port-feasibility-study.md` plan:

- restricted `C++11`
- headless CLI exporter
- deterministic `50 fps` / `22050 Hz` offline timeline
- Java-style intro script player for `mute95 -> domina -> filmbox`
- first autonomous `saari` 3D pass with script-row shock events
- first autonomous `kukot` 3D pass with sliced `jarnomix.xm` playback from `0x0700`
- first autonomous `maku` terrain pass with sliced `jarnomix.xm` playback from `0x0D00`
- first autonomous `watercube` mixed 3D / packed-surface pass with sliced `jarnomix.xm` playback from `0x1000`
- direct loading of original assets from `original/forward`
- native `512x256` uncompressed `TGA` frames
- one stereo `16-bit PCM` `WAV`
- `manifest.csv` kept close to the Java capture format
- `ffmpeg` wrappers for muxing

The default export path now runs on an audio-sample master clock:

- `--sequence intro` is the default
- `--intro-frames-per-row` and `--intro-rows-per-order` remain available as legacy wrapper hints, but they no longer drive scene/script timing
- `--sequence saari` exports the current direct-asset `saari` 3D pass with row-driven `suh0` / `suh` shock events
- `--sequence kukot` exports the current direct-asset `kukot` pass, starting from `jarnomix.xm` song position `0x0700`
- `--sequence maku` exports the current direct-asset `maku` terrain flythrough, starting from `jarnomix.xm` song position `0x0D00`
- `--sequence watercube` exports the current direct-asset `watercube` mixed 3D / packed-surface pass, starting from `jarnomix.xm` song position `0x1000`
- `--sequence bootstrap` keeps the older placeholder scene available for quick pipeline checks

Current limitation:

- `mute95` and `domina` are now structured as real C++ scene/routine ports with the Java message names and timing flow
- `mute95` now loads the original JPEG/GIF assets directly at runtime through vendored `stb_image` plus native GIF palette handling
- the current `mute95` render is already very close to the Java reference capture, with remaining drift concentrated around the central blue halo / horizontal band during the title phase
- `mute95` now keeps the blurred background cloud on a stable intensity envelope instead of letting the C++ port re-feed it into a late-sequence glow runaway; this matches the current Java baseline and the 2026 Power Mac G5 ground-truth screenshots more closely
- `domina` now loads `images/phorward.gif` directly as an indexed GIF and follows the Java `512x3840 -> 512x256` frame-strip scroll path
- `domina` late-frame comparisons are still influenced by the synthetic song-position transport, so the remaining drift there is not yet a pure scene-renderer verdict
- `intro` now renders native `kuninga.xm` audio and `saari` now renders native `jarnomix.xm` audio directly in C++, with no Java intermediation
- `saari` now loads `tai1sp.jpg`, `saari.gif`, `envi_klu.gif`, `saarih15.gif`, and `asses/alku6.ase` directly in C++ for a source-shaped terrain/object/reflection pass with deterministic `suh0` / `suh` shock handling
- the current `saari` port now mirrors the key Java trick: a camera-local terrain patch whose out-of-heightmap samples become flat water, with palette-masked additive reflections and no explicit `meditate` mirror copy
- `saari` now also follows the Java terrain visibility path more closely: heightmap samples clamp to non-negative land, terrain/env materials `3` and `259` are rasterized through a shared depth-sorted primitive batch, and the terrain quad face test now matches the original two-triangle slope logic
- `klunssi` reflection parity is source-driven here: Java keeps material `259` additive, but only on a one-shot reflective water mask; its mirrored concave face selection must follow the clone's mirrored object transform rather than a recomputed post-mirror world-space cross product; and the reflected clone does not inherit the original `klunssi` env-map `JAKkama` X-axis tweak
- `saari` is still not at final camera parity with Java, but the terrain/material path is now substantially closer to the original renderer than the earlier background-water approximation
- `kukot` now loads `asses/under1.ase`, `images/envplane.gif`, and `images/flare1.jpg` directly in C++, then renders a substantially closer source-shaped pass with cyclic squad-style quaternion interpolation closer to the Java `SplineTrack` path, Java-style `jAkKAma = 2` mesh deformation, normal-driven indexed-env material `3` lookup with affine screen-space interpolation like the original rasterizer, depth-sized additive flare blits, tiled noise background, scripted flash overlays, a Java-style left-to-right horizontal smear (`RgbSurface.aMajAKK(0.875f)`), and separate Java-style temporal frame ghosting (`RgbSurface.AmajakK()`) using packed-color math closer to the original surface code
- `kukot` flare rendering now also normalizes `flare1.jpg` from the original Java `<<20/<<10` packed RGB layout into standard `0xRRGGBB` before sprite blending; the earlier close-shot ring/cycle artifact was a packing mismatch, not a true additive overflow
- the current `kukot` port uses the real `jarnomix.xm` song-position slice from `0x0700`, so the standalone sequence and the full wrapper no longer restart the module from the beginning when entering that scene
- `kukot` keeps one documented local aesthetic override bundle: the repaired Java source does not explicitly flip normals or culling, yet side-by-side review converged on a substantially closer result when the C++ port reverses torsion direction, inverts body normals and culling winding, and remaps the env-map projection basis to `u <- 0.5 * (1 - z)`, `v <- 0.5 * (1 - y)`
- `maku` now loads `images/scape/loopk40.gif`, `images/scape/loopa2.gif`, and the camera tracks from `asses/vuori5.ase` directly in C++, then renders a first source-shaped repeating canyon flythrough with Java-like `go` / `speed` script messages, `suh` shock lines, rolling camera toggle support, default frame averaging, and the later `ksor` invert/smear feedback burst
- process note: the baseline `maku` conversion itself was completed in a single prompt from the current exporter state, and was materially more direct than the previous scene-port attempts; the remaining work is now fidelity tightening rather than first-pass structural translation
- the current `maku` pass is already in the right visual family against the frozen Java captures, but the exact timing of the washed-out/fog-heavy states and the later feedback cadence still need tightening
- `watercube` now loads `asses/nosto3.ase`, `images/1.jpg`, `images/txt1.jpg`, `images/reunus2.jpg`, `images/env3.jpg`, `images/rinku2.jpg`, `images/riple2.jpg`, and `meshes/kluns1.igu` / `meshes/kluns2.igu` directly in C++, then reproduces the native ripple ping-pong buffers, right-side panel, giant text overlay, and scripted `pum` / `rok` / `suh*` / `tex*` message flow
- the current `watercube` pass is already close to the frozen Java captures in broad composition, with the main remaining drift concentrated around env-mesh lighting and deeper Java face-mode `49` parity
- a light shared refactor is now in place for `ASE`/track parsing (`src/scenes/scene3d_shared.*`), reused by both `saari` and `kukot`; the rasterizers remain separate because `saari` still carries scene-specific terrain/reflection contracts that would make a broader 3D unification premature
- Java-based normalization helpers may still exist for validation, but they are not part of the exporter runtime path

## Build

From the repository root:

```powershell
cmake -S cpp-offline -B cpp-offline/build
cmake --build cpp-offline/build --config Release
```

On macOS, Apple Command Line Tools are sufficient:

```bash
xcode-select --install
cmake -S cpp-offline -B cpp-offline/build
cmake --build cpp-offline/build
```

## Run

```powershell
cpp-offline/build/forward-export --output cpp-offline/output --frames 250
```

macOS / POSIX shell form:

```bash
./cpp-offline/build/forward-export --output cpp-offline/output --frames 250
```

Longer intro validation run:

```powershell
cpp-offline/build/forward-export --output cpp-offline/output-intro --frames 1065 --intro-frames-per-row 1
```

Saari 3D pass run:

```powershell
cpp-offline/build/forward-export --sequence saari --output cpp-offline/output-saari --frames 3136 --intro-frames-per-row 7
```

Kukot 3D pass run:

```powershell
cpp-offline/build/forward-export --sequence kukot --output cpp-offline/output-kukot --until-song-position 0x0D00
```

Maku terrain pass run:

```powershell
cpp-offline/build/forward-export --sequence maku --output cpp-offline/output-maku --until-song-position 0x1000
```

Watercube mixed pass run:

```powershell
cpp-offline/build/forward-export --sequence watercube --output cpp-offline/output-watercube --until-song-position 0x1300
```

Generated output:

- `cpp-offline/output/frames/frame_000000.tga`
- `cpp-offline/output/audio/forward.wav`
- `cpp-offline/output/manifest.csv`
- `cpp-offline/output/log.txt`

Current audio status:

- `intro`, `saari`, `kukot`, `maku`, and `watercube` now write native stereo `16-bit PCM` module audio directly from `mods/kuninga.xm` and `mods/jarnomix.xm`.
- `kukot` now slices `jarnomix.xm` from its real handoff point (`0x0700`) before writing audio or song-position events.
- `maku` now slices `jarnomix.xm` from its real handoff point (`0x0D00`) before writing audio or song-position events.
- `watercube` now slices `jarnomix.xm` from its real handoff point (`0x1000`) before writing audio or song-position events.
- `bootstrap` still falls back to silence because it remains a placeholder scene outside the current preservation path.
- intro/saari/kukot/maku/watercube visual scripting now advances from the native XM song-position timeline derived from audio sample position.

## Mux with FFmpeg

Archive-quality:

```powershell
cpp-offline/scripts/mux_master.bat cpp-offline/output
```

macOS / POSIX shell form:

```bash
sh cpp-offline/scripts/mux_master.sh cpp-offline/output
```

Distribution copy:

```powershell
cpp-offline/scripts/mux_h264.bat cpp-offline/output
```

macOS / POSIX shell form:

```bash
sh cpp-offline/scripts/mux_h264.sh cpp-offline/output
```

Full current convenience wrapper:

```powershell
cpp-offline/scripts/export_intro_full.bat
```

macOS / POSIX shell form:

```bash
sh cpp-offline/scripts/export_intro_full.sh
```

The Windows wrapper uses the PowerShell merge script, and the POSIX wrapper uses `python3`, to combine the per-sequence exports into the final current-full output.

That wrapper:

- configures and builds `forward-export`
- resolves segment lengths from native XM song positions, then exports the complete current intro window through `0x1024` plus a short post-roll, followed by the current `saari` window through `0x0700`, the current `kukot` window through `0x0D00`, the current `maku` window through `0x1000`, the current `watercube` window through `0x1300`, the current `feta` window through `0x1600`, and a standalone `uppol` credits tail
- writes outputs under `cpp-offline/output-full-current`
- muxes `forward_full_current_master.mkv` and `forward_full_current_h264.mp4` when `ffmpeg` is available

## Standalone Release Packaging

From the repository root:

```powershell
package_forward_cpp_offline.bat
```

This builds the `Release` exporter, stages a standalone package under:

```text
cpp-offline/dist/forward-cpp-offline-win64
```

and also writes a zip archive next to it:

```text
cpp-offline/dist/forward-cpp-offline-win64.zip
```

The packaged folder includes:

- `forward-export.exe`
- `render_forward_full.bat`
- `scripts/merge_current_full_outputs.ps1`
- `scripts/mux_master.bat`
- `scripts/mux_h264.bat`
- `original/forward/...` with the original asset tree

The package is self-contained for the current C++ offline workflow: run `render_forward_full.bat` from the package root and it writes the offline export under `output/` by default.

## Immediate Next Porting Steps

1. Finish the source-faithful `mute95` validation against Java captures.
2. Tighten the remaining `saari` camera timing, depth-sort, and raster parity against the Java captures now that the terrain/material path is source-shaped.
3. Keep tightening `kukot` toward Java parity, with the next likely wins being near-plane clipping, later-shot flare occlusion, and any remaining quaternion-spline drift against the reference captures.
4. Tighten `maku` timing against the Java capture checkpoints now that the tiled terrain/camera path is source-shaped.
5. Tighten `watercube` env-mesh lighting, near-plane behavior, and Java face-mode `49` parity against the frozen captures now that the broad composition is source-shaped.
6. Reuse the new direct indexed GIF path for `uppol` and the remaining palette-driven routines.
7. Use the native XM sequencer / sample timeline to drive the remaining scene windows beyond the current `intro` / `saari` / `kukot` / `maku` / `watercube` scope, starting with `feta`.
