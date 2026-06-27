# Forward Java Desktop Reconstruction Process

## 1) Objective

This document records the practical reconstruction process used to turn the decompiled Java version of `forward` into a desktop-launchable Java application for Windows, instead of continuing the direct C++ preservation port as the primary path.

The goal of this work was:

- duplicate the existing Java code into a safe working tree
- restore enough source integrity to compile again
- remove the applet/browser dependency
- replace obsolete audio backends with a modern desktop audio path
- provide a local Windows launcher

## 2) Why the Approach Changed

The original plan in this repository focused on a C++ preservation port. That remains valid in principle, but the source material is heavily obfuscated and contains many era-specific hacks. In practice, the friction came from two directions at once:

- reverse engineering the original Java logic
- translating that logic into a new C++ runtime at the same time

The alternative path documented here reduces risk by staying inside the original language and architecture first, then modernizing the hosting layer around it.

In short:

- preserve Java engine structure first
- replace only the broken runtime assumptions
- keep behavior close to the shipped artifact before any cleanup or renaming

## 3) Source Material Used

This reconstruction was based on:

- `original/forward`
- `reverse/cfr_single`
- `reverse/procyon_single`
- `documentation/forward-portability-study-v2-from-decompiled-source.md`
- `documentation/forward-porting-effort-and-challenges.md`

The runtime assets are still taken from `original/forward`:

- `mods/*.xm`
- `images/**`
- `meshes/**`
- text and scene resources loaded by the original code

## 4) Working Tree Created

A separate Java reconstruction tree was created:

- `java-desktop/src/main/java`

This tree started as a copy of `reverse/cfr_single`, then was edited in place to become the desktop-compatible source base.

This separation is intentional:

- it preserves the raw reverse-engineered outputs in `reverse/`
- it avoids mixing reconstruction work with the C++ port under `port/`
- it creates a new canonical Java desktop work area

## 5) Reconstruction Workflow

### 5.1 Copy the decompiled source tree

The initial source base was copied from `reverse/cfr_single` into:

- `java-desktop/src/main/java`

This gave a complete but not yet compilable source tree.

### 5.2 Replace or repair decompiler failures

The copied tree did not compile as-is because several classes still contained broken control-flow artifacts such as:

- `** GOTO`
- `lbl-1000`
- partially structured loops

The most problematic classes were repaired by combining:

- direct source cleanup
- `javap -c -p` bytecode inspection
- targeted rewrites of only the broken methods

Key repaired classes included:

- `mmaamma`
- `kmajkka`
- `kmjjmmk`
- `kmjjkka`
- `kaajmma`
- `kmaamka`
- `kaajmka`

Additional decompilation typing fixes were required in:

- `forward`
- `kaajkka`
- `majjmka`
- `majjmma`

### 5.3 Remove applet-era hosting assumptions

The original runtime assumed:

- `java.applet.Applet`
- `AppletStub`
- `AppletContext`
- browser-driven lifecycle and resource lookup

Those assumptions were replaced with a desktop host model centered on AWT components.

Main hosting changes:

- `mmjamma` now extends `Panel` instead of `Applet`
- standalone launch still goes through `forward.main(...)`
- `kajjkmk` continues to provide the top-level frame
- `kaajkmk` became a lightweight parameter/base-url holder instead of an actual `AppletStub`
- `kmjakka` became a lightweight desktop context helper instead of an `AppletContext`
- `mmaakma` no longer depends on `Applet.getImage(...)`
- the original intermediate handoff to the screen-sized `mmjjmka` window was disabled so the demo stays in the same desktop window from start to finish

This keeps the structure close to the original code while removing dependence on APIs that are effectively dead for desktop use.

An additional timing correction was required for the `phorward.gif` backdrop used by `domina` and `uppol`.

In the original code, that backdrop advances from a frame counter, while the modern desktop loop can render much faster than the old expected cadence. Keeping the raw `++frameCounter` behavior made the backdrop race ahead of the music and scene script on current hardware.

For the desktop reconstruction, those two routines now derive their backdrop frame counter from scene time at a virtual `50 Hz` rate instead of the uncapped render loop. This preserves the intended pacing without changing the surrounding scene logic.

A temporary desktop-specific rendering cleanup had also been explored for textured materials `3` and `259`, which are heavily used by `saari`.

That experiment replaced the original affine per-triangle interpolation with a projective path to hide seams between adjacent triangles on modern captures.

For the current source-faithful desktop build, that cleanup has been reverted. Materials `3` and `259` now go through the original affine rasterizer path again, even if that keeps some of the era-authentic distortion visible.

The remaining `saari` sky issue is therefore not currently treated as a solved JDK-hosting problem. The strongest working hypothesis is now a subtler reconstruction/parity drift in low-level math, projection, or rasterization behavior. That investigation is tracked separately in:

- `documentation/forward-saari-sky-investigation.md`

### 5.4 Keep asset loading behavior close to the original release

The launcher intentionally uses `original/forward` as the runtime working directory.

This means:

- existing relative resource paths continue to work
- no asset migration was required for the first desktop rebuild
- the reconstructed Java code still uses the original shipped data set

This is the least risky preservation path for a first working desktop build.

### 5.5 Replace obsolete audio backends

The original audio device layer depended on:

- IE3/IE4 DirectSound classes under `com/ms/directX/*`
- `sun.audio`
- a no-sound fallback

Those backends are not usable on current standard JDKs.

The replacement strategy was:

- keep the `MAD` abstraction
- keep the existing call sites and device selection flow
- redirect the concrete devices to a new Java Sound implementation

Files introduced or simplified for this:

- `muhmu/hifi/device/DeviceJavaSound.java`
- `muhmu/hifi/device/DeviceMSbase.java`
- `muhmu/hifi/device/DeviceMS_IE3.java`
- `muhmu/hifi/device/DeviceMS_IE4.java`
- `muhmu/hifi/device/DeviceSun.java`
- `muhmu/hifi/device/DeviceNoSound.java`
- `muhmu/hifi/device/MAD.java`

Design choice:

- preserve device names for compatibility with the old logic
- provide one modern output path underneath
- keep `nosound` support

### 5.6 Keep obfuscated names for now

No broad renaming pass was attempted.

That is deliberate:

- the current priority is runnable preservation
- aggressive renaming at this stage would increase regression risk
- class and method identities are still useful when cross-checking against bytecode and decompilation notes

## 6) Files Added for the Desktop Path

The main user-facing additions are:

- `java-desktop/README.md`
- `run_forward_desktop.bat`

The batch script:

- collects all Java sources under `java-desktop/src/main/java`
- compiles them into `java-desktop/build/classes`
- switches to `original/forward`
- runs `forward` with any command-line arguments passed through

Supported historical options remain:

- `nosound 1`
- `1x1 1`

## 7) Build and Launch Procedure

From the repository root:

```bat
run_forward_desktop.bat
```

No-sound mode:

```bat
run_forward_desktop.bat nosound 1
```

High-resolution mode:

```bat
run_forward_desktop.bat 1x1 1
```

The build step is intentionally simple and dependency-free:

- no Gradle
- no Maven
- no external packaging tool

This keeps the reconstruction easy to inspect and easy to move between machines.

## 8) Validation Performed

The reconstructed source tree was compiled locally with:

- `javac 17.0.15`

Validation performed:

1. full source compilation of `java-desktop/src/main/java`
2. desktop launch through `run_forward_desktop.bat nosound 1`
3. desktop launch through `run_forward_desktop.bat`
4. short runtime observation to confirm the process stayed alive and did not immediately crash

Observed result:

- compilation succeeded
- both launch modes started successfully
- no immediate crash occurred in the first few seconds of runtime

This is enough to confirm that the desktop reconstruction path is viable.

## 9) Current Limitations

This is a working reconstruction path, not yet a fully cleaned preservation release.

Open limitations:

- the source tree is still heavily obfuscated
- compile output still reports deprecated and unchecked warnings
- no JAR packaging or installer exists yet
- no regression test harness compares output against the original applet
- no frame-accurate or audio-accurate preservation audit has been done yet
- validation was done locally on JDK `17.0.15`; newer JDKs should still be tested explicitly on Windows
- runtime assets are still read from `original/forward`

## 10) Recommended Next Steps

Recommended next actions are:

1. validate the desktop reconstruction on current Windows JDK releases beyond the local JDK 17 test
2. capture screenshots or video from both original and reconstructed builds for visual comparison
3. verify XM playback timing against expected scene transitions
4. decide whether to keep using the reconstructed Java mixer path as-is or replace it with a more maintainable module playback layer
5. package the desktop version as a repeatable deliverable
6. only then consider progressive deobfuscation and naming cleanup

## 11) Bottom Line

The Java desktop reconstruction path is now practical and implemented.

It does not solve every preservation problem, but it changes the project state from:

- "interesting reverse-engineering material"

to:

- "compilable desktop Java reconstruction with a local Windows launcher"

That is a materially better base for further preservation work than trying to reverse engineer and port to C++ at the same time.
