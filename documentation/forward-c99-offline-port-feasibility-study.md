# Forward C99 Offline Port Feasibility Study

## Scope

This study evaluates the feasibility of porting the current desktop Java source tree in `java-desktop/src/main/java` to:

- pure `C99`
- no separately installed runtime dependencies
- a native `512x256` export resolution
- a synchronized offline exporter that writes:
  - a TGA image stream
  - a WAV audio stream
- shell wrappers (`.bat` and `.sh`) that use `ffmpeg` to rebuild a video file
- a Windows `.exe` that can be cross-compiled from Linux
- a procedural C codebase derived from the current Java OOP implementation

This is not a study for a new interactive player. It is a study for a deterministic exporter. That distinction matters because it removes the need for a GUI, a real-time audio device, and platform-specific presentation code.

For this study, a vendored third-party single-file source dependency embedded directly in the repository is considered acceptable. The constraint is interpreted as "no external shared-library, package-manager, or separately installed runtime dependency".

## Executive Summary

The port is feasible, but only if the target is defined as a headless offline exporter rather than a native interactive desktop runtime.

The strongest implementation path is:

1. Use the current `java-desktop` code as the source of truth, not the original class files.
2. Keep the original software renderer and XM replay logic in C.
3. Replace all Java host services with a deterministic offline timeline.
4. Write uncompressed `TGA` frames and one `WAV` file directly from C.
5. Rebuild the final video with `ffmpeg` from external `.bat` and `.sh` scripts.

Under that model, the project is practical. The revised `512x256` native-output target matches the current logical render space of the Java desktop build and removes the largest previous resolution-scope risk.

Using a vendored single-header decoder such as `stb_image.h` for JPEG and GIF input also removes a large amount of low-value decoder work while keeping the final executable self-contained.

## Current Source Baseline

The current source baseline is materially better than porting directly from the original bytecode:

- `97` Java files
- about `15,730` lines of code
- `31` files directly tied to AWT/Swing/image hosting
- `13` audio-related files
- `9` asset I/O related files
- `6` subclasses of `mmjjmma` (main scene family)
- `2` subclasses of `majjkka` (overlay/text scene family)

Largest files by line count:

- `kajjmka.java`: `2512`
- `mmajmmk.java`: `776`
- `kaajmma.java`: `730`
- `forward.java`: `644`
- `mmaamma.java`: `600`
- `majjmka.java`: `587`
- `kaajkka.java`: `542`

This matters because the porting effort is not evenly distributed. A small set of renderer, mesh, script, and module-player files dominates the risk.

## Confirmed Java Architecture

### 1. Application shell and timeline

Main files:

- `forward.java`
- `mmjamma.java`
- `kajjmmk.java`
- `maaakka.java`
- `ForwardDesktopLauncher.java`
- `ForwardHostFrame.java`

What they do:

- parse applet-style parameters
- host the demo inside AWT
- maintain the main loop
- manage scene changes and script commands
- provide a smoothed wall-clock timer
- coordinate capture and presentation

### 2. Scene system

Base types:

- `mmjjmma`: main software-rendered scenes
- `majjkka`: overlay and text scenes

Scene implementations:

- `kmjjkmk`: `mute95`
- `maajmka`: `saari`
- `kmaamka`: `feta`
- `kajjkka`: `kukot`
- `kmajmka`: `watercube`
- `kmjjmka`: `maku`
- `kajakka`: `domina`
- `mmaakmk`: `uppol`

The scene list is fixed and known ahead of time. This is important because it means a procedural C port does not need a generic runtime object registry. A fixed enum and dispatch tables are enough.

### 3. Software rendering core

Main files:

- `mmaamma.java`
- `kmajkka.java`
- `mmajmmk.java`
- `kaajmma.java`
- `kaaakka.java`
- `kmjamka.java`
- `mmajkka.java`
- `mmjamka.java`

Observed characteristics:

- CPU-side rendering only
- direct pixel buffer manipulation
- both true-color and palette-indexed surfaces
- double-buffered software images
- custom copy, blend, fade, warp, palette, and raster routines
- 3D mesh rasterization and environment mapping

This is good news for C99. The renderer already behaves like a software engine, not like a Java 2D app.

### 4. Audio stack

Main files:

- `majjmka.java`: MOD/XM loader
- `maajmmk.java`: song player and pattern scheduler
- `majjmma.java`: mix bus and event callback bridge
- `mmjjkmk.java`: channel/voice player
- `muhmu/hifi/device/*`: device abstraction

Observed characteristics:

- the replay logic is already in Java code
- the current desktop build still uses the legacy `MAD` abstraction
- Java Sound is only the final output device
- the actual synthesis and sequencing logic is internal

This is the key reason a dependency-free WAV exporter is realistic. The project does not need an external XM library if the goal is preservation of current Java behavior.

### 5. Asset loading

Runtime assets under `original/forward` include:

- `2` XM modules
- `5` `.igu` mesh files
- `4` `.ase` scene files
- `22` `.jpg`
- `11` `.gif`

Loaders already exist for:

- `.xm` / `.mod`
- `.igu`
- `.ase`

What Java currently delegates to the runtime:

- GIF decoding
- JPEG decoding
- font metrics and text rendering
- image hosting and pixel transport

This is the main non-trivial part of a no-dependency C99 port.

## What Makes the Port Feasible

### 1. The target can be headless

The requirement is to generate image and audio streams, then call `ffmpeg` externally. That removes the need for:

- a native window
- a real-time display loop
- a real-time audio device
- input handling
- fullscreen management

All current Java desktop helpers under `Forward*` can be dropped from the C runtime.

### 2. The renderer is already software-first

The current Java code mostly renders into its own buffers, then presents them through AWT. That means the port can replace:

- `BufferedImage`
- `Graphics`
- `ImageProducer`
- `ImageConsumer`
- `Toolkit`

with direct memory-backed frame buffers and custom blitters.

### 3. The module player can run offline

The current Java audio path uses `Mixable`, `MAD`, and `bufferStartTime` to drive music timing. In C, the output does not need to be real-time. The engine can synthesize exact PCM blocks into a memory or file stream and expose the same timing information to the scene scheduler.

### 4. The scene set is fixed

The code is obfuscated, but it is not open-ended. The scene graph, file formats, and timeline commands are known. That keeps the port bounded.

## What Makes the Port Expensive

### 1. Image decoding still needs an explicit policy

The current source depends on Java for decoding:

- GIF
- JPEG

If "no dependency" means "no external runtime/install dependency", the problem is manageable and does not require writing custom JPEG/GIF decoders.

The best compromise is to vendor a single-file decoder directly in the source tree.

Recommended candidate:

- `stb_image.h`

Why it fits:

- it is a single embedded source file
- it supports JPEG and GIF decode in one place
- it can be configured to disable unused decoders
- it is dual-licensed under MIT or public domain terms

Relevant caveats from the upstream header:

- JPEG support covers baseline and progressive JPEG, but not 12-bit-per-channel or arithmetic-coded JPEG
- GIF decoding is supported, but the trivial loader path is still-image oriented; animated GIF handling needs the GIF-specific API path
- GIF decode returns RGBA-style pixel output rather than preserving the original palette at the public API boundary

For this repository, that looks acceptable. From code inspection, the Java runtime uses decoded GIF/JPEG assets as ordinary bitmaps after load, so still-image decoding appears sufficient. That last point is an inference from the current source tree rather than a direct statement from `stb`.

Recommended interpretation:

- no external runtime dependency for the final C executable
- vendored single-file third-party source is acceptable
- offline asset preconversion remains optional, not mandatory

### 2. The current Java desktop build is still logically `512x256`

The present Java desktop runtime uses:

- internal render logic centered on `512x256`
- display scaling for `1024x512`

This is visible in:

- `forward.java`
- `ForwardLaunchConfiguration.java`
- `ForwardFrameBuffer.java`

For the revised target, this is an advantage rather than a problem:

1. the exporter can write the native frame buffer directly as `512x256` TGA
2. no renderer-wide constant audit is required just to satisfy the output format

If a larger delivery format is needed later, it should be handled as optional `ffmpeg` post-processing, not as a first-phase renderer rewrite.

### 3. Procedural translation is mechanical but high-friction

Porting Java OOP to procedural C is possible, but it adds manual structure around:

- inheritance
- runtime dispatch
- ownership
- initialization order
- dynamic arrays and hash tables

This is still feasible because the class graph is modest and fixed, but it is slower than a near-1:1 C++ translation.

### 4. Timing must be redesigned

The Java desktop build still contains wall-clock behaviors:

- `System.currentTimeMillis()`
- `Thread.sleep(...)`
- smoothed frame timer in `maaakka`
- audio callback timing through device buffer timestamps

Those mechanisms must be replaced by one deterministic offline clock.

## Recommended Target Definition

The recommended deliverable is:

- `forward-export.exe`
- a headless CLI exporter
- no runtime dependencies
- no window
- no live audio playback
- no input handling
- output:
  - `frames/frame_000000.tga`
  - `audio/forward.wav`
  - optional `manifest.csv`
- external `.bat` and `.sh` scripts that call `ffmpeg`

This is the shortest path that satisfies all stated requirements.

## Recommended Synchronization Model

### Master principle

Use one integer timeline for both audio and video. Do not preserve the Java real-time loop.

### Best practical choice

Use:

- `50 fps` video
- `22050 Hz` stereo `16-bit` WAV as the default export rate

Why this fits the current Java desktop source:

- the current Java desktop audio init path uses `22050`
- several scene fixes already normalize frame-driven behavior to a virtual `50 Hz`
- `22050 / 50 = 441`, which gives an exact integer number of samples per frame

That means:

- frame `N` starts at sample `N * 441`
- there is no fractional drift
- audio and video remain perfectly aligned by construction

If a later parity pass restores `44100 Hz`, sync is still exact:

- `44100 / 50 = 882`

### Offline execution model

Recommended order per frame:

1. Compute target sample index for frame `N`.
2. Mix audio until that exact sample index.
3. Apply all music-driven and script-driven events whose timestamps are now due.
4. Evaluate scene time from the same unified timeline.
5. Render frame `N`.
6. Write `frame_%06d.tga`.

This replaces:

- `System.currentTimeMillis()`
- `Thread.sleep(...)`
- device-latency estimation
- asynchronous Java mixer threads

### Why audio should be the authoritative clock

The current Java code already lets the audio mixer drive event timestamps through `majjmma` and `maajmmk`. Keeping audio as the authority is the least risky way to preserve timing relationships.

## Recommended Rendering Strategy

### Native export path

The low-risk approach is:

- keep the original logical render space at `512x256`
- render exactly as the Java code does
- write that native frame buffer directly as `512x256` TGA

This is the cleanest preservation target because it matches the current engine assumptions.

### Optional downstream scaling

If a larger viewing copy is desired later:

- keep the C exporter native at `512x256`
- scale only in the external `ffmpeg` step
- treat scaling as presentation, not engine behavior

That keeps the renderer simpler and avoids introducing unnecessary visual regressions into the preservation build.

## Recommended Asset Strategy

### Runtime recommendation

Use two asset paths:

- native output path:
  - uncompressed `TGA` writer implemented locally in project code
- source-asset input path:
  - vendored `stb_image.h` for JPEG and GIF decoding

This keeps the exporter self-contained without forcing a custom image-decoder project into the critical path.

### Why `stb_image.h` is a good fit here

The upstream header explicitly supports:

- JPEG
- GIF
- TGA

That means one vendored decoder can cover:

- current repository source images
- optional future validation against TGA intermediates

Recommended integration rules:

- vendor the file under a clearly marked path such as `third_party/stb/`
- keep the original license text intact
- compile the implementation in exactly one translation unit
- disable unused format support where practical to reduce code surface
- keep the project's own TGA writer rather than adding `stb_image_write`

### Optional asset normalization

If deterministic preprocessing is still desired later, the build can normalize all source images to:

- uncompressed `TGA` for true-color assets
- optional custom palette/index blobs for assets where palette identity matters

This is aligned with the codebase because `mmaakma.java` already contains a manual image loader for TGA-like data, even though the current Java desktop build mainly uses AWT for JPG/GIF decoding.

That preprocessing step is now optional rather than required.

### Mesh and animation assets

Keep direct C ports of:

- `.igu` loader logic from `kajamka.java`
- `.ase` loader logic from `kaajkka.java`

The current repository uses plain `.ase`, not compressed `.asez`. GZIP support can therefore be deferred unless later source variants require it.

## Recommended C99 Procedural Architecture

### 1. Core data structures

Translate each major Java class family into:

- one `struct` for persistent state
- a set of namespaced functions

Examples:

- `forward.java` -> `forward_state_t` + `forward_init`, `forward_run_export`, `forward_shutdown`
- `mmaamma.java` -> `surface32_t` + blit/fade/blend functions
- `kmajkka.java` -> `surface8_t` + palette functions
- `maajmmk.java` -> `module_player_t`
- `mmjjkmk.java` -> `channel_t`

### 2. Runtime dispatch

For `mmjjmma` and `majjkka`, use explicit vtables:

```c
typedef struct scene_vtable {
    const char *name;
    void (*init)(void *state, struct forward_state *app);
    void (*destroy)(void *state);
    void (*on_show)(void *state);
    void (*on_hide)(void *state);
    void (*message)(void *state, const char *msg, float scene_time_s);
    void (*render)(void *state, struct surface32 *dst, float scene_time_s, float delta_s);
} scene_vtable_t;
```

This is procedural C, but it still preserves the current runtime behavior cleanly.

### 3. Collections

Do not port Java `Hashtable` and `Vector` literally unless needed.

Prefer:

- fixed arrays for known scene registries
- simple dynamic arrays for loader outputs
- enum-based dispatch for scene names
- small purpose-built lookup tables where asset names are static

The codebase is small enough that generic container complexity can be minimized.

### 4. Memory ownership

Recommended ownership model:

- explicit `init/free` pairs for every subsystem
- one app-level allocator policy
- optional arena allocators for temporary scene and mesh data

This is safer than trying to imitate Java lifetime rules.

## TGA and WAV Output Feasibility

### TGA

TGA output is straightforward in C99:

- fixed 18-byte header
- BGR or BGRA pixel dump
- no compression required

Recommended output:

- `24-bit` uncompressed TGA
- top-left or bottom-left origin chosen once and documented
- zero alpha unless a validation workflow needs it

### WAV

WAV output is also straightforward:

- `RIFF/WAVE`
- `PCM`
- stereo
- `16-bit`
- `22050 Hz` default, configurable later

The only care point is that the header needs the final data size. That is easy to solve by:

- writing a placeholder header
- streaming PCM data
- seeking back to patch sizes at the end

No external library is needed for either format.

## ffmpeg Reconstruction Workflow

The exporter should not invoke `ffmpeg` directly. Keep video rebuilding in external wrappers.

### Recommended output layout

- `output/frames/frame_000000.tga`
- `output/audio/forward.wav`
- `output/manifest.csv`
- `output/log.txt`

### Archive-quality rebuild example

```bat
ffmpeg -y ^
  -framerate 50 ^
  -i output\frames\frame_%%06d.tga ^
  -i output\audio\forward.wav ^
  -c:v ffv1 ^
  -level 3 ^
  -g 1 ^
  -c:a pcm_s16le ^
  output\forward_master.mkv
```

```sh
ffmpeg -y \
  -framerate 50 \
  -i output/frames/frame_%06d.tga \
  -i output/audio/forward.wav \
  -c:v ffv1 \
  -level 3 \
  -g 1 \
  -c:a pcm_s16le \
  output/forward_master.mkv
```

### Distribution rebuild example

```bat
ffmpeg -y ^
  -framerate 50 ^
  -i output\frames\frame_%%06d.tga ^
  -i output\audio\forward.wav ^
  -c:v libx264 ^
  -preset slow ^
  -crf 12 ^
  -pix_fmt yuv420p ^
  -c:a aac ^
  -b:a 192k ^
  output\forward_h264.mp4
```

```sh
ffmpeg -y \
  -framerate 50 \
  -i output/frames/frame_%06d.tga \
  -i output/audio/forward.wav \
  -c:v libx264 \
  -preset slow \
  -crf 12 \
  -pix_fmt yuv420p \
  -c:a aac \
  -b:a 192k \
  output/forward_h264.mp4
```

## Windows `.exe`, Built on Linux

This requirement is realistic for a headless C99 exporter.

### Why it is practical

- no GUI dependency
- no OS audio API
- no windowing API
- no graphics API
- only file I/O, memory, math, and command-line parsing

### Recommended toolchain

- native Linux build: `gcc` or `clang`
- Windows cross-build from Linux: `mingw-w64`

Example cross-build shape:

```sh
x86_64-w64-mingw32-gcc \
  -std=c99 -O2 -s \
  src/*.c \
  -o build/forward-export.exe
```

Platform-specific code should be kept to a tiny utility layer for:

- directory creation
- path separators
- optional large-file support

If threading is avoided in the exporter, the final `.exe` stays simpler and more portable.

## Required Java-to-C Translation Rules

To keep the port maintainable, the translation should follow strict rules.

### Recommended rules

1. Keep behavior first, naming second.
2. Preserve obfuscated identifiers in comments or mapping tables until parity is achieved.
3. Replace inheritance with explicit state structs and vtables.
4. Replace Java global singletons with explicit app-owned subsystems.
5. Remove all real-time threading from the first C exporter milestone.
6. Remove all desktop-only code from the runtime.
7. Keep loaders and renderer logic source-faithful before cleanup.

### What should not be ported literally

- `ForwardDesktopLauncher`
- `ForwardHostFrame`
- `ForwardStartupDialog`
- `ForwardDisplayOptions`
- Java Sound device plumbing
- AWT font and metrics code
- mouse-over URL behavior in `uppol`

Those are desktop-host concerns, not export-core concerns.

## Main Risks

### High risk

- exact parity of text scenes currently relying on Java font metrics
- preserving timing after removing wall-clock scheduling

### Medium risk

- manual ownership bugs introduced by procedural translation
- correct integration boundaries and configuration for vendored third-party image decode
- decompiler-era logic complexity in large files such as:
  - `kajjmka.java`
  - `mmajmmk.java`
  - `kaajmma.java`
  - `forward.java`
  - `mmaamma.java`
  - `majjmka.java`
  - `kaajkka.java`

### Low risk

- TGA writer
- WAV writer
- Linux -> Windows cross-compilation
- `ffmpeg` wrapper scripts

## Estimated Effort

For one experienced engineer working full-time:

### Single-phase preservation exporter, native `512x256` TGA frames

- `6 to 9 weeks`

Includes:

- procedural scaffolding
- software surfaces
- mesh and scene loaders
- XM replay port
- deterministic offline timeline
- TGA/WAV output
- `ffmpeg` scripts
- reference validation against current Java captures

That estimate assumes the current Java desktop source remains the functional reference.

No separate high-resolution renderer phase is required for the current target.

## Recommended Execution Order

1. Freeze the Java desktop version as the behavioral baseline.
2. Reuse the existing capture workflow to create reference outputs.
3. Build a minimal headless C harness that writes blank TGA and WAV files.
4. Port `mmaamma`, `kmajkka`, and related software surface helpers.
5. Port `.igu` and `.ase` loaders.
6. Port the XM loader and mixer stack.
7. Replace `kajjmmk` and wall-clock scheduling with an offline event queue.
8. Port the scene families one by one.
9. Generate native `512x256` TGA output directly from the software frame buffer.
10. Add `ffmpeg` scripts and document the workflow end to end.
11. If a larger delivery copy is needed, scale it in `ffmpeg`, not in the C renderer.

## Bottom Line

The project is feasible in pure C99 with no runtime dependencies, provided the target is a deterministic offline exporter and not a native interactive player.

The safest interpretation of the requirements is:

- port the current Java demo logic to procedural C
- export synchronized native `512x256` `TGA` + `WAV`
- rebuild video outside the executable with `ffmpeg`
- cross-compile a Windows `.exe` from Linux

With `512x256` confirmed as the native export target, the study becomes materially simpler: the exporter can stay aligned with the current Java engine's logical frame size, and any later enlargement can remain an external video-assembly concern.
