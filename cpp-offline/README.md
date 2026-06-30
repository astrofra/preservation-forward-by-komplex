# Forward C++11 Offline Scaffold

This directory contains the first usable milestone of the `documentation/forward-cpp11-subset-offline-port-feasibility-study.md` plan:

- restricted `C++11`
- headless CLI exporter
- deterministic `50 fps` / `22050 Hz` offline timeline
- Java-style intro script player for `mute95 -> domina -> filmbox`
- direct loading of original assets from `original/forward`
- native `512x256` uncompressed `TGA` frames
- one stereo `16-bit PCM` `WAV`
- `manifest.csv` kept close to the Java capture format
- `ffmpeg` wrappers for muxing

The default export path now runs the intro sequence through a synthetic song-position transport:

- `--sequence intro` is the default
- `--intro-frames-per-row 6` approximates the classic `6 ticks per row` pace at `50 fps`
- `--sequence bootstrap` keeps the older placeholder scene available for quick pipeline checks

Current limitation:

- `mute95` and `domina` are now structured as real C++ scene/routine ports with the Java message names and timing flow
- `mute95` now loads the original JPEG/GIF assets directly at runtime through vendored `stb_image` plus native GIF palette handling
- the current `mute95` render is already very close to the Java reference capture, with remaining drift concentrated around the central blue halo / horizontal band during the title phase
- the intro zoom-noise path may still be slightly oversaturated; we do not currently have a reliable capture of the original Java runtime to confirm whether that saturation is correct
- `domina` still needs direct indexed GIF loading for the full source-faithful path
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

Generated output:

- `cpp-offline/output/frames/frame_000000.tga`
- `cpp-offline/output/audio/forward.wav`
- `cpp-offline/output/manifest.csv`
- `cpp-offline/output/log.txt`

## Mux with FFmpeg

Archive-quality:

```powershell
cpp-offline/scripts/mux_master.bat cpp-offline/output
```

Distribution copy:

```powershell
cpp-offline/scripts/mux_h264.bat cpp-offline/output
```

Full intro convenience wrapper:

```powershell
cpp-offline/scripts/export_intro_full.bat
```

That wrapper:

- configures and builds `forward-export`
- exports the complete current intro window through `0x1024` plus a short post-roll
- writes outputs under `cpp-offline/output-intro-full`
- muxes `forward_intro_master.mkv` and `forward_intro_h264.mp4` when `ffmpeg` is available

## Immediate Next Porting Steps

1. Finish the source-faithful `mute95` validation against Java captures.
2. Add direct indexed GIF loading for `phorward.gif` and move `domina` off its stand-in renderer.
3. Replace the synthetic song-position transport with the real XM sequencer.
4. Replace silent WAV generation with the native XM replay path.
