# Forward C++11 Subset Offline Port Feasibility Study

## Scope

This study evaluates the feasibility of porting the current desktop Java source tree in `java-desktop/src/main/java` to:

- a restricted `C++11` codebase
- no separately installed runtime dependencies
- a native `512x256` export resolution
- a synchronized offline exporter that writes:
  - a TGA image stream
  - a WAV audio stream
- shell wrappers (`.bat` and `.sh`) that use `ffmpeg` to rebuild a video file
- a Windows `.exe` that can be cross-compiled from Linux
- a codebase that stays close to the original Java object model where that improves fidelity and maintenance

This is the same delivery target as the `C99` study, but with a different implementation language constraint.

For this study, a vendored third-party single-file source dependency embedded directly in the repository is considered acceptable. The constraint is interpreted as "no external shared-library, package-manager, or separately installed runtime dependency".

Runtime requirement clarification:

- the native exporter must load the original repository assets directly
- Java-assisted preprocessing or Java-generated intermediate assets may be used for validation during development, but not as a required runtime step for the C++ exporter

Current implementation direction:

- vendorizing `stb_image.h` is the intended runtime path for direct source-asset loading in the exporter
- Java-side helpers may still be kept in the repository for reference capture, decode validation, or regression comparison, but not as a mandatory preprocessing stage
- for `Saari`, the native exporter should stay close to the Java terrain/material trick: a camera-local terrain patch, off-heightmap fallback to flat water, and reflection gating driven by palette-derived masking rather than by a separate ocean mesh
- the current `Saari` parity work also shows that renderer-level details matter: Java clamps terrain heights to non-negative land, sorts terrain/env primitives by average depth instead of z-buffering them, and uses affine material `3` / `259` rasterization for this scene
- `klunssi` reflection parity is a good example of why the port must preserve renderer contracts, not just scene composition: material `259` stays additive, but only through a one-shot water mask, mirrored-face visibility rules that prevent concave self-overlap in the original demo, and a reflected clone that does not inherit the original env-map tweak matrix
- the recent Saari terrain recovery has been written up as a reusable parity-debugging reference in `documentation/forward-saari-terrain-parity-methodology.md`
- the recent `maku` baseline conversion is also a useful process datapoint: from the current exporter state it landed in a single prompt and was materially more direct than the previous scene-port attempts, which suggests that some later scene windows may now be blocked more by fidelity iteration than by baseline translation friction

## Executive Summary

This port is feasible, and under the current constraints a restricted `C++11` codebase is a better fit than pure `C99`.

The reason is structural, not ideological:

- the current Java source is object-centric
- scene dispatch is already polymorphic
- renderer and mesh code keep state and behavior tightly coupled
- the audio and timeline layers already rely on object ownership and mutation patterns that map naturally to classes

For the specific target in this repository, the best path is:

1. Keep the exporter headless and deterministic.
2. Keep the native render size at `512x256`.
3. Preserve the current Java class structure where it helps parity.
4. Use a narrow `C++11` subset instead of broad modern C++.
5. Vendor `stb_image.h` for JPEG/GIF input and keep TGA/WAV writing in project code.

Compared with the `C99` path, `C++11` reduces translation friction, reduces manual ownership code, and lowers the risk of behavior regressions introduced only by the language change.

## Current Source Baseline

The current Java desktop tree remains the correct source baseline:

- `97` Java files
- about `15,730` lines of code
- `31` files directly tied to AWT/Swing/image hosting
- `13` audio-related files
- `9` asset I/O related files
- `6` subclasses of `mmjjmma`
- `2` subclasses of `majjkka`

Largest files by line count:

- `kajjmka.java`: `2512`
- `mmajmmk.java`: `776`
- `kaajmma.java`: `730`
- `forward.java`: `644`
- `mmaamma.java`: `600`
- `majjmka.java`: `587`
- `kaajkka.java`: `542`

That profile favors a language that can preserve existing type boundaries and runtime dispatch without forcing a procedural rewrite everywhere.

## Why C++11 Is a Better Fit Than C99 Here

### 1. The Java code is already organized around classes

Main examples:

- `mmjjmma` and `majjkka` define scene families
- `mmajmmk` holds mesh state plus behavior
- `mmaamma` and `kmajkka` are stateful surface types
- `maajmmk`, `majjmma`, and `mmjjkmk` form a layered audio engine

In `C99`, these become:

- structs
- manual vtables
- explicit init/free discipline
- more glue code around ownership and dispatch

In `C++11`, they can stay as:

- classes
- constructors and destructors
- virtual methods where already implied by the Java design
- container-backed members for arrays and owned resources

### 2. The requested target is still headless

The project does not need:

- a GUI
- a live window
- a real-time audio device
- input handling

That means `C++11` is not being proposed to justify a heavyweight runtime. It is only being used to reduce translation cost and improve code structure.

### 3. A restricted subset keeps complexity under control

This should not become a "modern C++ framework" rewrite. The recommended approach is a narrow subset that behaves more like "better C with classes" than like a large template-heavy application.

## Recommended C++11 Subset

### Allowed language features

- classes and structs
- constructors and destructors
- single inheritance
- virtual methods where Java already relied on polymorphism
- references
- `enum class`
- `override`
- `nullptr`
- range-for where it improves clarity
- move support only where obviously useful

### Allowed standard library features

- `std::vector`
- `std::array`
- `std::string`
- `std::unique_ptr`
- `std::map` or `std::unordered_map` only for places that genuinely need keyed lookup
- fixed-width integer types from `<cstdint>`

### Recommended restrictions

- no Boost
- no exceptions for ordinary control flow
- no multiple inheritance
- no RTTI-dependent design
- no metaprogramming-heavy abstractions
- no smart-pointer graphs with shared ownership unless strictly necessary
- no dependency on platform GUI or audio APIs in the core exporter

### Practical coding rule

Use `C++11` to preserve the Java engine shape, not to redesign it.

## Confirmed Architecture Fit

### 1. Application shell and timeline

Main Java files:

- `forward.java`
- `mmjamma.java`
- `kajjmmk.java`
- `maaakka.java`

Recommended `C++11` mapping:

- `forward_state` becomes a `forward_app` class
- script scheduling remains its own class
- the smoothing timer becomes an offline timeline helper
- desktop hosting classes under `Forward*` disappear from the runtime

### 2. Scene system

Main Java abstraction:

- `mmjjmma`
- `majjkka`

Recommended `C++11` mapping:

- preserve these as two abstract base classes
- preserve each scene as a concrete derived class
- keep `show`, `message`, and `render` entrypoints close to their Java meaning

This is one of the biggest advantages over `C99`. In this codebase, the existing class graph is not accidental. Preserving it makes the port easier to validate.

### 3. Software renderer

Main Java files:

- `mmaamma.java`
- `kmajkka.java`
- `kaajmma.java`
- `mmajmmk.java`
- `kaaakka.java`
- `kmjamka.java`

Recommended `C++11` mapping:

- one surface class for true-color buffers
- one surface class for indexed buffers and palette state
- mesh, face, vector, and UV types as lightweight value classes
- renderer entrypoints kept close to Java names until parity is achieved

This part of the code is already software-rendered and CPU-bound. `C++11` does not change that architecture. It only makes the translation less mechanical.

### 4. Audio engine

Main Java files:

- `majjmka.java`
- `maajmmk.java`
- `majjmma.java`
- `mmjjkmk.java`

Recommended `C++11` mapping:

- module loader and player remain native engine code
- no external XM runtime is required
- channel objects can remain classes with internal state
- mix buses can own channel objects through containers

Again, this is structurally simpler than procedural translation.

## Same Output Model as the C99 Study

The exporter target should remain identical:

- headless
- deterministic
- native `512x256` frame buffer
- uncompressed `TGA` frame sequence
- one `WAV` file
- external `ffmpeg` scripts for final muxing

There is no technical reason to make the `C++11` study interactive if the `C99` study already established that offline export is the cleanest fit.

## Synchronization Model

The synchronization model should remain the same as in the `C99` study.

### Recommended clock

Use one offline master timeline derived from audio sample position.

Recommended defaults:

- `50 fps`
- `22050 Hz`
- stereo
- `16-bit PCM`

Why this works well:

- `22050 / 50 = 441`
- each video frame corresponds to an exact integer number of samples
- no fractional drift is introduced

### Frame generation order

Recommended frame loop:

1. Compute the target sample index for frame `N`.
2. Mix audio until that exact sample boundary.
3. Dispatch all due script and music-driven events.
4. Evaluate scene time from the same unified timeline.
5. Render frame `N`.
6. Write `frame_%06d.tga`.

This fully replaces:

- Java wall-clock timing
- `Thread.sleep(...)`
- audio-device latency estimation
- asynchronous desktop mixer behavior

## Asset Strategy

### Runtime recommendation

Use:

- local project code for TGA writing
- local project code for WAV writing
- vendored `stb_image.h` for JPEG and GIF decoding

This keeps the final executable self-contained while avoiding unnecessary decoder work.

For the current `cpp-offline` track, this is not only an allowed option but the preferred runtime architecture: original repository assets are to be decoded natively by the exporter itself.

### Why `stb_image.h` still fits

It remains a good match here for the same reasons as in the `C99` study:

- single embedded source file
- supports JPEG and GIF decode
- can be kept in `third_party/stb/`
- no installed dependency on the target machine

Relevant caveats:

- JPEG support covers baseline and progressive JPEG, but not every rare JPEG variant
- GIF decoding is supported, but animated GIF handling uses the GIF-specific API path
- public decode output is RGBA-oriented rather than palette-preserving

That appears acceptable for this repository. From code inspection, the current Java runtime treats loaded GIF/JPEG assets as decoded bitmaps after load. That is an inference from the source tree rather than a statement from `stb`.

### Optional asset normalization

If later validation benefits from fully normalized assets, the build can still preconvert source media to:

- uncompressed `TGA`
- optional custom palette/index blobs for assets where palette identity matters

That step should remain optional, not mandatory.

## Cross-Compilation and Deliverable Shape

This requirement remains practical in `C++11`.

### Why it is still easy

- no GUI
- no platform audio backend
- no graphics API
- only file I/O, memory management, and command-line control

### Recommended toolchains

- Linux native build: `g++` or `clang++`
- Linux -> Windows cross-build: `x86_64-w64-mingw32-g++`

Example build shape:

```sh
x86_64-w64-mingw32-g++ \
  -std=c++11 -O2 -s \
  src/*.cpp \
  -o build/forward-export.exe
```

If the code stays inside the recommended subset, no unusual compiler support is required.

## Recommended Project Layout

One practical layout:

- `src/core/`
- `src/audio/`
- `src/render/`
- `src/assets/`
- `src/app/`
- `src/platform/`
- `third_party/stb/`
- `scripts/`

The platform layer should stay tiny:

- path helpers
- directory creation
- optional large-file handling

The exporter core should remain platform-agnostic.

## Main Risks

### High risk

- exact parity of text scenes currently relying on Java font metrics
- timing differences introduced while removing wall-clock behavior
- preserving behavior in the largest stateful renderer and audio files

### Medium risk

- over-modernizing the design instead of preserving current structure
- using too much standard-library indirection in hot paths
- correct integration and configuration of vendored image decode
- decompiler-era complexity in large files such as:
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

## What C++11 Improves Over C99

### 1. Lower translation overhead

The Java code can be ported more directly:

- class to class
- method to method
- base type to base type

This reduces the amount of non-behavioral code written only to emulate the source language.

### 2. Better ownership hygiene

`std::vector`, `std::array`, and `std::unique_ptr` remove a large amount of manual allocation and cleanup code that would otherwise exist in `C99`.

### 3. Easier parity debugging

When the translated code preserves the original object boundaries, it is easier to compare Java and C++ behavior class by class.

### 4. Lower regression risk in scene dispatch

Because scene polymorphism is already explicit in Java, keeping that model in `C++11` is less risky than replacing it with manual dispatch tables everywhere.

## Estimated Effort

For one experienced engineer working full-time:

### Single-phase preservation exporter, native `512x256` TGA frames

- `4 to 7 weeks`

Includes:

- exporter scaffolding
- software surfaces
- mesh and scene loaders
- original-style XM replay port
- deterministic offline timeline
- TGA/WAV output
- `ffmpeg` scripts
- reference validation against the current Java desktop build

That estimate assumes the current Java desktop tree remains the behavioral reference.

Compared with the `C99` study, the expected schedule improvement comes mainly from:

- less manual OOP emulation
- less ownership boilerplate
- fewer translation-only regressions

## Recommended Execution Order

1. Freeze the Java desktop version as the behavioral baseline.
2. Reuse the existing capture workflow to create reference outputs.
3. Build a minimal headless `C++11` harness that writes blank TGA and WAV files.
4. Port the core surface and math classes first.
5. Port `.igu` and `.ase` loaders.
6. Port the XM loader, channel logic, and mix bus.
7. Port the timeline and script scheduler.
8. Port the scene families with their original class boundaries intact.
9. Generate native `512x256` TGA output directly from the software frame buffer.
10. Add `ffmpeg` scripts and document the workflow end to end.

## Recommendation

Under the same delivery constraints, a restricted `C++11` exporter is the stronger option than pure `C99`.

It keeps the same preservation shape:

- headless
- deterministic
- no separately installed runtime dependency
- native `512x256` TGA output
- WAV output
- external `ffmpeg` muxing
- Linux -> Windows cross-compilation

But it removes a substantial amount of translation friction introduced only by the `C99` language boundary.

## Bottom Line

If the goal is strictly "same output constraints, but not strictly procedural C", then `C++11` is the more practical implementation target.

It preserves the Java architecture more faithfully, reduces porting overhead, and still allows a small, dependency-light Windows exporter that can be built from Linux.

For this repository, the best balance is:

- offline exporter, not interactive player
- native `512x256` output
- vendored single-file image decode
- original audio logic kept in-engine
- restricted `C++11` rather than broad modern C++
