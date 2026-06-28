# Forward Java Desktop

Desktop Java reconstruction base for `forward`, derived from the decompiled sources in `reverse/cfr_single`.

## What Has Been Modernized

- removed the `Applet` dependency in favor of an AWT desktop host
- replaced the old `IE3/IE4` / `sun.audio` audio backends with `Java Sound`
- fixed decompiled classes that still contained `GOTO` artifacts
- disabled the intermediate switch to a full-screen window so the whole demo stays in the same desktop window
- retimed the `phorward.gif` scroll in `domina` and `uppol` to a virtual `50 Hz` cadence to avoid speeding up on modern machines
- retimed the frame-driven parts of `mute95` from scene time to limit intro overexposure without breaking warp fluidity
- retimed the frame-driven animations in `watercube` to a virtual `50 Hz` cadence so the center rotation, ripple, and `rok` damping stay aligned with the original binary
- restored the original affine rasterizer for Java materials `3` / `259` to remain source-faithful in `saari`
- locked AWT text rendering to an explicit monospace font with antialiasing disabled so text screens stay consistent between the source launcher and the `jpackage` build

## Prerequisites

- a JDK available in `PATH`

## Launching

From the repository root:

```bat
run_forward_desktop.bat
```

Without interactive launch arguments, the desktop launcher now opens a small startup GUI where you can choose:

- `Windowed`
- `Fullscreen`
- `Native 512x256`
- `X2 1024x512`
- `1x1 pixel mode`

`Fullscreen` keeps a black background across the entire screen and centers the demo at the selected size.
`1x1 pixel mode` matches the original Java binary flag `1x1 1`. It is now enabled by default in the desktop port.

Historical options are still supported:

```bat
run_forward_desktop.bat nosound 1
run_forward_desktop.bat 1x1 1
run_forward_desktop.bat 1x1 0
run_forward_desktop.bat nosound 1 1x1 1
```

Additional desktop options:

```bat
run_forward_desktop.bat launcher 0 displaymode windowed displayscale 2
run_forward_desktop.bat launcher 0 displaymode fullscreen displayscale 1
```

Parameters:

- `launcher 0`: skip the startup GUI
- `displaymode windowed|fullscreen`: force the display mode
- `displayscale 1|2`: change only the final on-screen presentation size
- `1x1 1|0`: force `1x1` mode on or off

The historical `1x1 1` flag still controls the internal rendering mode. `1x1 0` forces the older reduced mode. `displayscale` affects only on-screen presentation.

The script compiles `java-desktop/src/main/java` into `java-desktop/build/classes`, then launches `ForwardDesktopLauncher` using `original/forward` as the working directory so the original assets can be reused.

## Win64 Packaging

A `jpackage` workflow is now available to produce a standalone Windows build with an embedded Java runtime:

```bat
package_forward_desktop.bat
```

Default output:

```text
java-desktop\dist\jpackage\app-image\Forward\Forward.exe
```

`Forward.exe` does not require a JDK to be installed on the target machine.

The packaged build uses the same launcher GUI as the source build, with the same `displaymode`, `displayscale`, and `launcher 0` options.

Optional Windows installer:

```bat
package_forward_desktop.bat exe
```

Installer generation requires WiX in `PATH`. The plain `app-image` only depends on the JDK.

The detailed workflow is documented in:

- `documentation/forward-jpackage-workflow.md`

## Reference Capture

The desktop build can now capture itself to PNG through `key value` parameters.

Example:

```bat
run_forward_desktop.bat capture documentation\reference-capture\java captureintervalms 2000 capturelimit 60 captureexit 1
```

Capture mode automatically skips the startup GUI. Captures remain in native `512x256` resolution even if interactive display was selected in `x2`.

Ready-to-use wrappers:

```bat
capture_forward_demo.bat
capture_reference_video.bat
```

Outputs:

- `documentation/reference-capture/java/manifest.csv`
- `documentation/reference-capture/java/frames/*.png`

The full Java capture + video extraction + comparison workflow is documented in:

- `documentation/forward-reference-capture-workflow.md`

## Saari Diagnostics

A dedicated investigation note for the `saari` sky is maintained in:

- `documentation/forward-saari-sky-investigation.md`

The desktop build also exposes a temporary diagnostic switch to compare several interpretations of the backdrop mapping:

```bat
set JAVA_TOOL_OPTIONS=-Dforward.saariBackdropUvMode=procedural
```

Available values:

- `procedural`
- `mesh`
- `spherical`

The default mode remains `procedural`. These options exist only for the `saari` fidelity investigation.

Numeric probe available:

```bat
probe_saari_sky_original.bat
probe_saari_sky_java_desktop.bat
compare_saari_sky_probe.bat
```

The workflow and CSV outputs are documented in:

- `documentation/forward-saari-probe-workflow.md`

Currently confirmed result for `SCENE_TIME_MS=144000` in `procedural` mode:

- backdrop UVs, projected vertices, and visible `saari` backdrop triangles are identical between `original` and `java-desktop`
- a real reconstruction mismatch was fixed in `kaajmma.MajAkKa(float)` to restore the original bytecode `f2l; l2i` semantics
- the probe `backdrop_raster_preview.png` is now pixel-identical between `original` and `java-desktop` at the `144000 ms` checkpoint
