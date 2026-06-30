# Forward C++11 Offline Scaffold

This directory contains the first usable milestone of the `documentation/forward-cpp11-subset-offline-port-feasibility-study.md` plan:

- restricted `C++11`
- headless CLI exporter
- deterministic `50 fps` / `22050 Hz` offline timeline
- Java-style intro script player for `mute95 -> domina -> filmbox`
- first autonomous `saari` 3D pass with script-row shock events
- direct loading of original assets from `original/forward`
- native `512x256` uncompressed `TGA` frames
- one stereo `16-bit PCM` `WAV`
- `manifest.csv` kept close to the Java capture format
- `ffmpeg` wrappers for muxing

The default export path now runs on an audio-sample master clock:

- `--sequence intro` is the default
- `--intro-frames-per-row` and `--intro-rows-per-order` remain available as legacy wrapper hints, but they no longer drive scene/script timing
- `--sequence saari` exports the current direct-asset `saari` 3D pass with row-driven `suh0` / `suh` shock events
- `--sequence bootstrap` keeps the older placeholder scene available for quick pipeline checks

Current limitation:

- `mute95` and `domina` are now structured as real C++ scene/routine ports with the Java message names and timing flow
- `mute95` now loads the original JPEG/GIF assets directly at runtime through vendored `stb_image` plus native GIF palette handling
- the current `mute95` render is already very close to the Java reference capture, with remaining drift concentrated around the central blue halo / horizontal band during the title phase
- the intro zoom-noise path may still be slightly oversaturated; we do not currently have a reliable capture of the original Java runtime to confirm whether that saturation is correct
- `domina` now loads `images/phorward.gif` directly as an indexed GIF and follows the Java `512x3840 -> 512x256` frame-strip scroll path
- `domina` late-frame comparisons are still influenced by the synthetic song-position transport, so the remaining drift there is not yet a pure scene-renderer verdict
- `intro` now renders native `kuninga.xm` audio and `saari` now renders native `jarnomix.xm` audio directly in C++, with no Java intermediation
- `saari` now loads `tai1sp.jpg`, `saari.gif`, `envi_klu.gif`, `saarih15.gif`, and `asses/alku6.ase` directly in C++ for a first terrain/object/reflection pass with deterministic `suh0` / `suh` shock handling
- that `saari` renderer is now genuinely 3D (heightmap terrain, reflective water background, ASE camera/object parsing), but it is still not the final source-faithful camera/raster parity pass from the roadmap
- Java-based normalization helpers may still exist for validation, but they are not part of the exporter runtime path

## Build

From the repository root:

```powershell
cmake -S cpp-offline -B cpp-offline/build
cmake --build cpp-offline/build --config Release
```

## Run

```powershell
cpp-offline/build/forward-export --output cpp-offline/output --frames 250
```

Longer intro validation run:

```powershell
cpp-offline/build/forward-export --output cpp-offline/output-intro --frames 1065 --intro-frames-per-row 1
```

Saari 3D pass run:

```powershell
cpp-offline/build/forward-export --sequence saari --output cpp-offline/output-saari --frames 3136 --intro-frames-per-row 7
```

Generated output:

- `cpp-offline/output/frames/frame_000000.tga`
- `cpp-offline/output/audio/forward.wav`
- `cpp-offline/output/manifest.csv`
- `cpp-offline/output/log.txt`

Current audio status:

- `intro` and `saari` now write native stereo `16-bit PCM` module audio directly from `mods/kuninga.xm` and `mods/jarnomix.xm`.
- `bootstrap` still falls back to silence because it remains a placeholder scene outside the current preservation path.
- intro/saari visual scripting now advances from the native XM song-position timeline derived from audio sample position.

## Mux with FFmpeg

Archive-quality:

```powershell
cpp-offline/scripts/mux_master.bat cpp-offline/output
```

Distribution copy:

```powershell
cpp-offline/scripts/mux_h264.bat cpp-offline/output
```

Full current convenience wrapper:

```powershell
cpp-offline/scripts/export_intro_full.bat
```

That wrapper:

- configures and builds `forward-export`
- exports the complete current intro window through `0x1024` plus a short post-roll, then the current `saari` window through `0x0700`
- writes outputs under `cpp-offline/output-full-current`
- muxes `forward_full_current_master.mkv` and `forward_full_current_h264.mp4` when `ffmpeg` is available

## Immediate Next Porting Steps

1. Finish the source-faithful `mute95` validation against Java captures.
2. Tighten `saari` camera timing, orientation, and raster parity against the Java captures.
3. Reuse the new direct indexed GIF path for `uppol` and the remaining palette-driven routines.
4. Use the native XM sequencer / sample timeline to drive the remaining scene windows beyond the current `intro` / `saari` scope.
5. Extend the native audio path from the current `intro` / `saari` scope to the remaining real sequences as they land.
