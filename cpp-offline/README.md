# Forward C++11 Offline Scaffold

This directory contains the first usable milestone of the `documentation/forward-cpp11-subset-offline-port-feasibility-study.md` plan:

- restricted `C++11`
- headless CLI exporter
- deterministic `50 fps` / `22050 Hz` offline timeline
- native `512x256` uncompressed `TGA` frames
- one stereo `16-bit PCM` `WAV`
- `manifest.csv` kept close to the Java capture format
- `ffmpeg` wrappers for muxing

The current scene is a placeholder bootstrap scene. It exists to validate the exporter pipeline before porting the real Java renderer, scheduler, and audio engine.

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

1. Replace `PlaceholderScene` with real scene base classes ported from Java.
2. Port the software surfaces and raster helpers into `src/render/`.
3. Port the timeline and script scheduler into `src/app/`.
4. Replace silent WAV generation with the native XM replay path.
