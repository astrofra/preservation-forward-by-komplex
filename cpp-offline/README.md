# Forward C++11 Offline Scaffold

This directory contains the first usable milestone of the `documentation/forward-cpp11-subset-offline-port-feasibility-study.md` plan:

- restricted `C++11`
- headless CLI exporter
- deterministic `50 fps` / `22050 Hz` offline timeline
- Java-style intro script player for `mute95 -> domina -> filmbox`
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
- their visuals are still procedural stand-ins until the exact JPEG/GIF and palette-preserving asset path is wired in

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

## Immediate Next Porting Steps

1. Replace the procedural intro stand-ins with palette-preserving loads of the real `krad3.gif`, `phorward.gif`, and credit JPEGs.
2. Replace the synthetic song-position transport with the real XM sequencer.
3. Port the remaining shared raster helpers from the Java surfaces into `src/render/`.
4. Replace silent WAV generation with the native XM replay path.
