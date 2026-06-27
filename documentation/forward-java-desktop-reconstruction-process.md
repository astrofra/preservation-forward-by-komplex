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
- a dedicated `ForwardDesktopLauncher` was added for packaged builds so bundled assets can still be resolved through the original `getDocumentBase()` / relative URL logic without depending on the process working directory
- the text-only AWT overlays now resolve an explicit monospace font (`Courier New` first on Windows, then controlled fallbacks) and disable text antialiasing to keep the source launcher and packaged standalone build visually aligned

This keeps the structure close to the original code while removing dependence on APIs that are effectively dead for desktop use.

An additional timing correction was required for the `phorward.gif` backdrop used by `domina` and `uppol`.

In the original code, that backdrop advances from a frame counter, while the modern desktop loop can render much faster than the old expected cadence. Keeping the raw `++frameCounter` behavior made the backdrop race ahead of the music and scene script on current hardware.

For the desktop reconstruction, those two routines now derive their backdrop frame counter from scene time at a virtual `50 Hz` rate instead of the uncapped render loop. This preserves the intended pacing without changing the surrounding scene logic.

The same class of problem also showed up in `watercube`.

`kmajmka.maJakkA(...)` originally advanced several scene states once per render:

- the local rotation of the two env-mapped center meshes
- the ripple injection / ping-pong simulation
- the `rok` camera-roll damping (`kAMaJak *= 0.917f`)

On the desktop build, capture index `61` (`244002 ms`, scene time `16085 ms`) already landed at render frame `65474`, which is far above the cadence expected by the 1998 Java release. Leaving those updates tied to the uncapped render loop made the center object spin too fast and sped up the water response as well.

For the desktop reconstruction, `watercube` now advances those frame-based states from scene time on the same virtual `50 Hz` cadence. This keeps the scene logic source-faithful while restoring the original pacing on modern hardware.

`mute95` required the same kind of desktop pacing correction as well.

In that intro scene, the palette-warp background and star-like noise buildup were authored as frame-driven updates:

- the tile-warp phase counter advanced once per rendered frame
- the noise field injected `220` random brightening writes per rendered frame
- the accumulated palette field was then filtered and displayed

At the uncapped desktop framerates observed during capture, that caused the intro to over-accumulate brightness and wash out the centered title far earlier than in the 1998 reference video.

For the desktop reconstruction, the strictly frame-driven parts of `mute95` now derive their rate from scene time instead of the uncapped render loop. In practice, that means the random brightening writes and the small frame-phase jitter no longer scale with raw desktop fps, while the main warp motion still follows the original continuous `f2`-driven update. This keeps the particles fluid while constraining the over-accumulation seen on modern hardware.

The separate `krad3.gif` palette hypothesis was also tested directly and ruled out: raw GIF palette entries, indexed pixels, and palette-index histogram match exactly between the file on disk and the current Java desktop runtime loader. That result is documented in:

- `documentation/forward-mute95-palette-investigation.md`

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

For packaging, the same rule still applies, but the assets are staged into the application image instead of being read directly from the repository checkout.

The packaged launcher computes the staged asset root at runtime and injects it through the existing `basedir` parameter path. That avoids a broad resource-loader rewrite while still producing a self-contained Windows deliverable.

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
- `package_forward_desktop.bat`
- `probe_saari_sky_original.bat`
- `probe_saari_sky_java_desktop.bat`
- `compare_saari_sky_probe.bat`

The batch script:

- collects all Java sources under `java-desktop/src/main/java`
- compiles them into `java-desktop/build/classes`
- switches to `original/forward`
- runs `forward` with any command-line arguments passed through

The packaging script:

- recompiles the same Java source tree
- builds a runnable JAR with `ForwardDesktopLauncher` as entry point
- stages the runtime asset directories from `original/forward` (`asses`, `images`, `meshes`, `mods`)
- runs `jpackage` to create a Windows `app-image`
- can also produce an installer `exe` when WiX is available

Supported historical options remain:

- `nosound 1`
- `1x1 1`

Additional parity-investigation helpers now exist for `saari`:

- `probe_saari_sky_original.bat`
- `probe_saari_sky_java_desktop.bat`
- `compare_saari_sky_probe.bat`

Those wrappers are documented in:

- `documentation/forward-saari-probe-workflow.md`

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
- no external packaging tool for the development launcher

This keeps the reconstruction easy to inspect and easy to move between machines.

For distributable Windows builds, `jpackage` is now the preferred path. The detailed wrapper and runtime layout are documented in:

- `documentation/forward-jpackage-workflow.md`

## 8) Validation Performed

The reconstructed source tree was compiled locally with:

- `javac 17.0.15`

Validation performed:

1. full source compilation of `java-desktop/src/main/java`
2. desktop launch through `run_forward_desktop.bat nosound 1`
3. desktop launch through `run_forward_desktop.bat`
4. short runtime observation to confirm the process stayed alive and did not immediately crash
5. numeric `saari` probe dump from the desktop build
6. numeric `saari` probe dump from the original bytecode build
7. automatic comparison of both probe outputs
8. Windows packaging smoke build with `jpackage`

Observed result:

- compilation succeeded
- both launch modes started successfully
- no immediate crash occurred in the first few seconds of runtime
- the `saari` probe wrappers ran successfully for both original and desktop Java paths
- at `t = 144000 ms`, original and desktop probes matched exactly for backdrop projected vertices and visible triangles
- a bytecode/source mismatch in `kaajmma.MajAkKa(float)` was found and corrected
- after that correction, the probe backdrop raster preview also matched pixel-for-pixel between original and desktop at the same checkpoint
- the source tree is now also ready to produce a self-contained Windows app image without relying on a locally installed target-machine JDK

This is enough to confirm that the desktop reconstruction path is viable, and that at least one critical `saari` geometry checkpoint already matches the original runtime numerically.

## 9) Current Limitations

This is a working reconstruction path, not yet a fully cleaned preservation release.

Open limitations:

- the source tree is still heavily obfuscated
- compile output still reports deprecated and unchecked warnings
- the package is not signed
- no regression test harness compares output against the original applet
- no frame-accurate or audio-accurate preservation audit has been done yet
- validation was done locally on JDK `17.0.15`; newer JDKs should still be tested explicitly on Windows
- runtime assets are still mirrored from `original/forward`

## 10) Recommended Next Steps

Recommended next actions are:

1. validate the desktop reconstruction on current Windows JDK releases beyond the local JDK 17 test
2. capture screenshots or video from both original and reconstructed builds for visual comparison
3. verify XM playback timing against expected scene transitions
4. decide whether to keep using the reconstructed Java mixer path as-is or replace it with a more maintainable module playback layer
5. validate the packaged `app-image` on a clean Windows machine
6. decide whether to sign the packaged build and/or produce an installer as part of release workflow
7. only then consider progressive deobfuscation and naming cleanup

## 11) Bottom Line

The Java desktop reconstruction path is now practical and implemented.

It does not solve every preservation problem, but it changes the project state from:

- "interesting reverse-engineering material"

to:

- "compilable desktop Java reconstruction with a local Windows launcher"

That is a materially better base for further preservation work than trying to reverse engineer and port to C++ at the same time.
