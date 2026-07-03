# Forward C++ Offline macOS Compilation Audit

## Scope

This audit evaluates the work required to compile `cpp-offline` on modern macOS (historically "OS X").

The assessment was performed on **July 3, 2026** on:

- `macOS 14.1`
- `arm64`
- `Apple clang 15.0.0`
- `CMake 3.31.3`

The goal of this study is narrow:

- verify whether `cpp-offline` configures on macOS
- identify the concrete compilation blockers
- estimate the implementation effort needed to make macOS a supported build target

This is not a full visual-parity or performance audit.

## Executive Summary

`cpp-offline` is already very close to building on macOS.

The current build system configures successfully on macOS with no changes. The native build then fails on one portability issue: the code uses `sscanf_s`, which is a Microsoft-specific secure CRT function and is not provided by AppleClang/libc on macOS.

After neutralizing that single issue in a probe build by aliasing `sscanf_s` to `sscanf`, the project:

- compiled successfully
- linked successfully
- launched successfully
- produced short test exports for `bootstrap`, `intro`, `saari`, and `kukot`

That result strongly suggests the macOS compilation effort is **small**.

## Build Evidence

### 1. CMake configuration on macOS

The project configured successfully with the existing build files:

```bash
cmake -S cpp-offline -B cpp-offline/build-macos-audit
```

No macOS-specific CMake failure was observed.

### 2. First native build attempt

The first build failed here:

- `cpp-offline/src/scenes/saari_scene.cpp:730`
- `cpp-offline/src/scenes/saari_scene.cpp:741`
- `cpp-offline/src/scenes/saari_scene.cpp:750`
- `cpp-offline/src/scenes/saari_scene.cpp:792`
- `cpp-offline/src/scenes/saari_scene.cpp:825`

Observed compiler error:

```text
error: use of undeclared identifier 'sscanf_s'
```

### 3. Probe build after removing the MSVC-only blocker

To see whether more macOS issues were hidden behind the first failure, a probe build was run with:

```bash
cmake -S cpp-offline -B cpp-offline/build-macos-probe -DCMAKE_CXX_FLAGS='-Dsscanf_s=sscanf'
cmake --build cpp-offline/build-macos-probe --config Release
```

That probe build completed successfully.

### 4. Probe runtime validation

The resulting executable was able to run short exports on macOS:

```bash
./cpp-offline/build-macos-probe/forward-export --sequence bootstrap --output cpp-offline/output-macos-probe-bootstrap --frames 2
./cpp-offline/build-macos-probe/forward-export --output cpp-offline/output-macos-probe-intro --frames 1
./cpp-offline/build-macos-probe/forward-export --sequence saari --output cpp-offline/output-macos-probe-saari --frames 1
./cpp-offline/build-macos-probe/forward-export --sequence kukot --output cpp-offline/output-macos-probe-kukot --frames 1
```

Each command completed successfully and wrote frames plus a WAV file.

## Findings

### 1. Primary compilation blocker: `sscanf_s`

The current blocker is not architectural. It is a direct API portability mistake.

`sscanf_s` appears in:

- `cpp-offline/src/scenes/saari_scene.cpp`
- `cpp-offline/src/scenes/scene3d_shared.cpp`

Relevant call sites found during the audit:

- `saari_scene.cpp:730, 741, 750, 792, 825`
- `scene3d_shared.cpp:367, 379, 389, 436, 476`

Why this fails on macOS:

- `sscanf_s` is part of the Microsoft secure CRT family
- AppleClang on macOS provides standard `sscanf`, not `sscanf_s`
- the current parser format strings only use `%d` and `%f`, so standard `sscanf` is sufficient

Impact:

- compilation stops before linking
- macOS support currently fails even though the rest of the codebase is largely portable

### 2. The blocker exists in both active and dead code

The `scene3d_shared.cpp` parser helpers are actively used by `kukot_scene.cpp` and parts of `saari_scene.cpp`.

`saari_scene.cpp` also contains an older local copy of similar parsing helpers. Those local helpers appear to be unused now, but they still compile, so they still break the macOS build.

This matters because a partial fix in only one file would be incomplete.

### 3. The rest of the build is already cross-platform enough

Several areas that often cause macOS trouble are already in acceptable shape:

- `CMakeLists.txt` does not link against Windows-only libraries
- the executable is pure C++ with no Cocoa, Carbon, DirectX, SDL, OpenGL, or platform SDK dependency
- `src/platform/file_utils.cpp` already has a `_WIN32` split and a POSIX path for `mkdir`
- the muxing scripts already include POSIX shell variants: `scripts/mux_master.sh` and `scripts/mux_h264.sh`

In other words, the project does not need a macOS host-layer rewrite.

### 4. Current AppleClang warnings are non-blocking

The probe build emitted warnings, but not additional errors:

- unused helpers in `saari_scene.cpp`
- unused helpers in `kukot_scene.cpp`
- one warning inside vendored `stb_image.h`

These do **not** currently prevent compilation because the build does not use `-Werror`.

They are worth cleaning up only if the project wants stricter CI later.

### 5. Documentation is Windows-biased even though the code is nearly portable

`cpp-offline/README.md` currently documents build and run commands in PowerShell form only, for example:

- `cmake -S cpp-offline -B cpp-offline/build`
- `cmake --build cpp-offline/build --config Release`
- batch wrapper usage

Those commands mostly work on macOS as well, but the README does not explicitly say:

- that Apple Command Line Tools are enough
- that macOS is expected to work
- that `.sh` wrappers exist for muxing

This increases the apparent porting risk even though the actual code change is small.

## Required Work

### Minimal work to make `cpp-offline` compile on macOS

1. Replace `sscanf_s` with a portable alternative in both `saari_scene.cpp` and `scene3d_shared.cpp`.
2. Rebuild with AppleClang.
3. Run at least one smoke export per main sequence.

Recommended implementation choices:

- simplest: replace `sscanf_s(...)` with `std::sscanf(...)`
- cleaner: add one small internal wrapper and use that wrapper everywhere

Estimated effort:

- **0.5 day** for an engineer already familiar with the tree

Risk level:

- **low**

### Recommended work to make macOS a maintained target

1. Remove or refactor the duplicate legacy parsing helpers in `saari_scene.cpp`.
2. Add explicit macOS build instructions to `cpp-offline/README.md`.
3. Add a macOS CI job that runs `cmake` configure + build.
4. Keep one short smoke-export command in CI or a documented manual verification checklist.

Estimated effort:

- **1 to 2 days**

Risk level:

- **low**

### Optional cleanup work

1. Reduce AppleClang warning noise from unused local helpers.
2. Consider suppressing third-party warnings from `stb_image.h` if stricter warning settings are introduced later.
3. Decide whether `saari_scene.cpp` should continue carrying local parser copies now that `scene3d_shared.*` exists.

Estimated effort:

- **0.5 to 1 day**

This work is not required for basic macOS compilation.

## Suggested Patch Strategy

The safest short-term patch is:

1. change all `sscanf_s` calls to `std::sscanf`
2. ensure `<cstdio>` remains included where needed
3. rebuild on macOS
4. run smoke exports for `bootstrap`, `intro`, `saari`, and `kukot`

A slightly better medium-term patch is:

1. introduce one internal parsing helper in `scene3d_shared.*` or a small utility header
2. route both shared and scene-local parsing through that helper
3. delete unused duplicate parsing code from `saari_scene.cpp`

The second option reduces the chance of reintroducing Windows-only scanning calls later.

## Estimated Total Effort

For the narrow goal "make it compile on macOS":

- **small effort**
- **likely same-day fix**

For the stronger goal "treat macOS as a supported platform":

- **1 to 2 engineering days**

That estimate includes:

- code fix
- local verification
- README update
- basic CI coverage

## Bottom Line

`cpp-offline` does not need a major macOS port.

The current failure is caused by a small set of MSVC-only `sscanf_s` calls. Once those are replaced with a portable scanning path, the project builds and runs on macOS in probe testing. The practical work required is therefore modest: one small code fix for immediate compilation, plus a short follow-up pass for documentation and CI if macOS should remain supported over time.
