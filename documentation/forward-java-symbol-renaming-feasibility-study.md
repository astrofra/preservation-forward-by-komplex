# Forward Java Symbol Renaming Feasibility Study

## Scope

This study covers the current Java desktop reconstruction under `java-desktop/src/main/java`.
The goal is to estimate how far the obfuscated source can be renamed safely by inferring symbol intent from code structure, asset names, script strings, and runtime behavior.

The assessment is based on:

- the current source tree in `java-desktop/src/main/java`
- the current capture implementation in `ForwardFrameCapture`
- the existing workflow notes in:
  - `documentation/forward-java-desktop-reconstruction-process.md`
  - `documentation/forward-reference-capture-workflow.md`
  - `documentation/forward-global-script-timeline.md`

I also verified that the current tree compiles on `OpenJDK 17.0.15`.

## Executive Summary

Feasibility is high.

A large part of the source can be renamed safely, provided the work is staged and backed by automated capture-based regression checks.

The main conclusion is:

- class names are the easiest high-value target
- scene lifecycle methods are also strongly inferable
- shared renderer, math, loader, and orchestration symbols are mostly recoverable
- scene-private fields and locals are the main hard area

In practice, a first readability pass can rename most top-level concepts with low risk:

- scene classes
- scene base classes
- timeline orchestration symbols in `forward`
- framebuffer and image classes
- math primitives
- mesh, camera, and loader classes
- module loader and mixer abstractions

The remaining ambiguity is concentrated in:

- scene-private accumulators
- short-lived fixed-point scratch variables
- effect-specific state named only by author shorthand such as `suh`, `rok`, `pum`, or `ksor`

## Why This Is Feasible

The current tree already contains enough semantic anchors to infer many names with confidence.

### 1. The scene registry is explicit

`forward.java` contains the global script command array and clearly maps scene names to classes:

- `mute95` -> `kmjjkmk`
- `domina` -> `kajakka`
- `saari` -> `maajmka`
- `kukot` -> `kajjkka`
- `maku` -> `kmjjmka`
- `watercube` -> `kmajmka`
- `feta` -> `kmaamka`
- `uppol` -> `mmaakmk`

This already gives eight safe scene-class renames.

### 2. The abstract scene lifecycle is obvious

The two scene bases have very regular method shapes:

- `mmjjmma`: off-screen scene rendered into `mmaamma`
- `majjkka`: direct routine rendered into `Graphics`

The lifecycle methods are consistently used as:

| Legacy method | Inferred meaning |
|---|---|
| `majakkA()` / `amAjAkk()` | `scriptName()` |
| `MajakkA(mmjamma)` / `AmAjAkk(mmjamma)` | `init(...)` |
| `MAjakkA()` / `AMAjAkk()` | `onShow()` |
| `mAJakkA()` / `aMaJaKK()` | `dispose()` |
| `maJakkA(...)` / `amaJaKK(...)` | `render(...)` |
| `MAJakkA(String, float)` / `AMaJaKK(String, float)` | `handleMessage(...)` |

These are strong, repeatable, low-risk renames.

### 3. Asset paths reveal subsystem roles

The code names many assets directly:

- `mods/*.xm`
- `images/...`
- `meshes/*.igu`
- `asses/*.ase`

That makes several classes immediately recognizable:

- `majjmka`: module loader
- `kaajkka`: ASE loader
- `kajamka`: IGU mesh loader
- `kmajmka`: `watercube`
- `kmjjkmk`: `mute95`

### 4. The math and rendering primitives are structurally obvious

Several core types have unmistakable behavior:

- `mmajmma`: 3D vector math
- `maajkka`: 3x3 matrix or basis transform
- `mmjakka`: vertex with position and projected data
- `kmajkmk`: UV coordinate
- `kmaamma`: triangle or face
- `mmajmmk`: mesh object
- `kaajmmk`: camera
- `kaajmma`: textured triangle rasterizer
- `mmaamma`: RGB software surface
- `kmajkka`: indexed or paletted surface

### 5. The current tree already proves selective deobfuscation works

Readable support classes have already been introduced successfully:

- `ForwardDesktopLauncher`
- `ForwardLaunchConfiguration`
- `ForwardFrameBuffer`
- `ForwardFrameCapture`
- `ForwardHostFrame`
- `ForwardStartupDialog`

This matters because it shows the codebase can tolerate incremental readability improvements without forcing a full rewrite.

## Current Scope Snapshot

Observed in the current workspace:

- `97` Java source files in total
- `78` top-level files still use obfuscated names
- roughly `683` obfuscated method names remain in the current tree
- the current tree compiles successfully on JDK 17

These numbers are large enough to require process, but small enough for an incremental campaign.

## High-Confidence Rename Targets

The following class-level renames are strongly supported by code behavior and usage patterns.

| Legacy class | Proposed name | Why the name is justified |
|---|---|---|
| `forward` | `ForwardDemoApp` | main runtime, script interpreter, scene/audio/capture orchestration |
| `mmjamma` | `DesktopAppletBase` | applet-like parameter and base-url host adapted to desktop |
| `kaajkmk` | `AppletParameterStub` | stores parameters and base URL |
| `kmjakka` | `DesktopAppletContext` | desktop replacement for applet context operations |
| `maaakka` | `SmoothedFrameTimer` | moving-average timer used for frame timing |
| `mmjjmma` | `Scene` | abstract off-screen scene lifecycle |
| `majjkka` | `GraphicsRoutine` | abstract direct-to-`Graphics` routine lifecycle |
| `mmajkka` | `SoftwareImageSurface` | base `ImageProducer` wrapper |
| `mmjamka` | `ImageDecodeConsumer` | `ImageConsumer` used to decode AWT images into engine surfaces |
| `mmaamma` | `RgbSurface` | 32-bit RGB software framebuffer |
| `kmajkka` | `IndexedSurface` | 8-bit paletted software surface |
| `mmajkmk` | `SurfacePresenter` | common presenter and scaler base |
| `kaaakka` | `RgbSurfacePresenter` | presents `RgbSurface` content |
| `kmjamka` | `IndexedSurfacePresenter` | presents `IndexedSurface` content |
| `mmajmma` | `Vec3f` | 3D vector operations |
| `maajkka` | `Mat3f` | 3x3 basis or transform matrix |
| `mmjakka` | `Vertex` | 3D vertex with projected attributes |
| `kmajkmk` | `UvCoord` | texture coordinates |
| `kmaamma` | `Triangle` | face made from three vertices and three UVs |
| `mmajmmk` | `MeshObject` | transformable mesh or object instance |
| `kaajmmk` | `Camera` | view origin, orientation, near/far, FOV |
| `kaajmka` | `ViewFrustum` | frustum setup and mesh visibility tests |
| `kmaakma` | `SceneRenderer` | collects objects, culls, sorts, renders |
| `maaakma` | `DepthSorter` | quicksort-like sorter over primitives by depth |
| `kaajmma` | `TexturedTriangleRasterizer` | textured triangle drawing into `RgbSurface` |
| `kaajkka` | `AseSceneLoader` | parses `.ase` scene and object data |
| `kajamka` | `IguMeshLoader` | parses `.igu` mesh data |
| `majjmka` | `ModuleLoader` | XM/MOD loader |
| `maajmmk` | `ModuleSong` | loaded module or song model |
| `majjmma` | `MixerBus` | mixer and dispatcher of `Mixable` sources |
| `kajjkmk` | `ExitOnCloseFrame` | legacy close-to-exit frame |

Scene classes can be renamed immediately:

| Legacy class | Proposed name |
|---|---|
| `kmjjkmk` | `Mute95Scene` |
| `kajakka` | `DominaRoutine` |
| `maajmka` | `SaariScene` |
| `kajjkka` | `KukotScene` |
| `kmjjmka` | `MakuScene` |
| `kmajmka` | `WatercubeScene` |
| `kmaamka` | `FetaScene` |
| `mmaakmk` | `UppolRoutine` |

## High-Confidence Targets Inside `forward`

`forward.java` contains many symbols that are inferable from control flow alone.

| Legacy symbol | Proposed name | Reason |
|---|---|---|
| `kkAmajA` | `scriptCommands` | global MuhmuScript command array |
| `kKaMAjA` | `scriptCursor` | current index into `scriptCommands` |
| `KKaMAjA` | `nextScriptTimeHex` | next scheduled script time marker |
| `KkAmajA` | `deferredScriptCommand` | command attached to a future `_xxxx` marker |
| `kKAmajA` | `sceneRegistry` | `mmjjmma` map keyed by script name |
| `KkamajA` | `routineRegistry` | `majjkka` map keyed by script name |
| `KKamAJA` | `activeScene` | active `mmjjmma` instance |
| `kKamAJA` | `activeRoutine` | active `majjkka` instance |
| `kkAMAJA` | `frameTimer` | timer used for scene time and frame pacing |
| `KkAMAJA` | `renderFrame` | incremented every render loop |
| `kKAMAJA` | `sceneTimeSeconds` | scene time in seconds |
| `KKAMAJA` | `deltaSeconds` | frame delta in seconds |
| `KKaMAJA` | `showDebugTimecode` | on-screen debug time overlay toggle |
| `KamAjak()` | `advanceTimeline()` | advances script execution |
| `KaMajak(String)` | `executeScriptCommand(String)` | command dispatcher |
| `kAMAJak(String)` | `showScene(String)` | activates scene or routine |
| `KAMajak(String)` | `killScene(String)` | removes scene or routine |

This file alone can gain a large amount of readability without any logic rewrite.

## Medium-Confidence Rename Targets

These should be renamed only after the class map and scene lifecycle are already cleaned up:

- shared renderer fields inside `mmajmmk`, `kmaakma`, `kaajmma`, `mmaamma`, and `kmajkka`
- camera and frustum intermediate methods in `kaajmmk` and `kaajmka`
- loader helper methods in `kaajkka` and `kajamka`
- audio and player internals in `majjmka`, `majjmma`, `mmjjkmk`, and related classes

The reason is not that these are unknowable.
It is that they are dense, stateful, and easy to rename incorrectly in a way that still compiles.

## Low-Confidence Rename Targets

These should be last:

- scene-private fields in `mute95`, `saari`, `watercube`, `feta`, and `maku`
- fixed-point scratch variables inside rasterizers and palette code
- local variables in long render methods
- counters whose meaning changes between phases of the same scene

For these, comments may be better than forced renames until the effect has been fully understood.

## Main Risks

### 1. Case-only obfuscation patterns

Many symbols differ only by case.
Manual search and replace is unsafe.

Use structured refactoring only:

- IntelliJ rename refactor
- Eclipse rename refactor
- AST-aware rewrite tooling

### 2. Reflection-based class lookup

Some code resolves classes by string:

- `majjmka` uses `Class.forName("muhmu.gl.ZipHoax")`
- `muhmu.hifi.device.MAD` uses fully qualified device class names

If any renamed class is referenced by string, the string literal must be updated in the same change set.

### 3. Decompiled hot spots still exist

Some files were already repaired manually after decompilation issues.
Those areas should be treated as behavior-sensitive.

In practice:

- keep renames separate from logic cleanup
- do not rewrite control flow in the same commit as a naming pass

### 4. Default-package coupling

Most of the engine still lives in the default package.
That makes cross-file package-private access common.

This does not block renaming, but it increases the value of automated refactors and compile gates.

## Recommended Renaming Strategy

### Phase 0: Freeze a baseline

Before renaming anything:

1. capture the current build output as the behavioral baseline
2. generate a symbol map file
3. define confidence levels for every proposed rename

Recommended artifacts:

- `documentation/forward-symbol-map.csv`
- `documentation/reference-capture/baseline/...`

Suggested symbol map columns:

- legacy symbol
- proposed symbol
- kind: class, method, field, or local
- confidence: high, medium, or low
- evidence
- files touched
- status

### Phase 1: Rename only top-level concepts

First pass:

- scene class names
- scene base classes
- scene lifecycle methods
- `forward` orchestration symbols
- math, mesh, camera, and loader class names

This is the highest readability gain per unit of risk.

### Phase 2: Rename shared engine methods

Second pass:

- surface operations
- loader entry points
- renderer entry points
- camera and frustum methods
- mixer entry points

Keep each patch narrowly scoped to one subsystem.

### Phase 3: Rename shared engine fields

Third pass:

- state that is used across multiple files
- fields whose meaning is visible at call sites

Good targets are:

- width and height
- timer state
- active scene and routine references
- camera parameters
- mesh transforms
- buffer arrays

### Phase 4: Scene-by-scene deep cleanup

Only after the engine skeleton is readable:

- open one scene at a time
- rename its fields and locals
- validate the scene in isolation with capture windows

Recommended order:

1. `mute95`
2. `watercube`
3. `saari`
4. `feta`
5. `maku`
6. `kukot`
7. `domina`
8. `uppol`

### Phase 5: Optional package reorganization

Do not move everything into packages during the naming campaign.

If package cleanup is desired, do it later, after:

- class names are stabilized
- capture baselines are in place
- symbol mapping is complete

## Regression Tracking Methodology

The Java desktop version already contains the right primitive for this job:

- frame capture to PNG
- per-frame manifest rows
- timestamps at both demo and scene scope
- scene name
- next script time marker

That is enough to build a robust no-regression pipeline for rename-only work.

### Key principle

For a pure renaming pass, the expected output is semantic identity.

That means the regression oracle should be:

- same scene order
- same script schedule
- same captured frames, or near-identical frames when timing jitter is unavoidable

### Proposed test tiers

#### Tier 0: Compile gate

Run on every change:

- compile the whole tree

This is already easy because `run_forward_desktop.bat` recompiles the project every time.

#### Tier 1: Fast capture smoke test

Run on every rename branch or pull request:

```bat
run_forward_desktop.bat ^
  launcher 0 ^
  nosound 1 ^
  capture documentation\regression\smoke ^
  captureintervalms 10000 ^
  capturelimit 8 ^
  captureexit 1
```

Purpose:

- confirm the app still boots
- confirm the capture pipeline still works
- confirm the script timeline still advances

Use `nosound 1` only for this fast smoke tier.

#### Tier 2: Audio-on baseline comparison

Run on every merged rename batch:

```bat
run_forward_desktop.bat ^
  launcher 0 ^
  capture documentation\regression\baseline-check ^
  captureintervalms 2000 ^
  capturelimit 60 ^
  captureexit 1
```

Audio should stay enabled here.
The existing workflow notes already warn that `nosound 1` can distort scene timing enough to skip sections of the demo.

#### Tier 3: Scene-window captures

Run on the subsystem currently being renamed.

Example approach:

- capture only the `mute95` time window while renaming `kmjjkmk`
- capture only the `watercube` time window while renaming `kmajmka`

The scene boundaries already exist in `documentation/forward-global-script-timeline.md`.

### What to compare automatically

The comparison should happen in two stages.

#### A. Manifest-level checks

Before looking at images, validate:

- same number of captures
- same scene sequence
- monotonic `demo_time_ms`
- monotonic `scene_time_ms` inside one scene
- same or very close `next_script_time_hex` progression

This catches timeline drift before pixel analysis.

#### B. Image-level checks

For aligned frames, compute:

- exact pixel equality when possible
- mean absolute error
- RMSE
- PSNR or SSIM

Store failure artifacts:

- side-by-side image
- absolute diff image
- CSV metrics
- Markdown summary

### Recommended frame alignment rule

Use the manifest, not the visible overlay, as the primary alignment source.

Best matching key:

1. `scene`
2. nearest `demo_time_ms` within tolerance
3. fallback to nearest `scene_time_ms`
4. verify `next_script_time_hex` still matches the expected region

This is more reliable than raw frame index because frame rate can vary.

### Important capture recommendation

Keep the on-screen debug timecode disabled for automated image diffs.

The current code already stores all important time markers in `manifest.csv`:

- `demo_time_ms`
- `scene_time_ms`
- `scene`
- `next_script_time_hex`

Use the manifest as metadata and keep the frame pixels clean.

## Gap in the Current Workspace

The workflow document references a Python comparison tool:

- `tools/forward_reference_compare.py`

But that script is not present in the current workspace.

So the capture-based regression strategy is feasible now, but not yet fully productized in this checkout.

Recommended follow-up:

implement a local comparison tool that can do both of these jobs:

1. compare current Java output against a stored Java baseline
2. optionally compare Java output against extracted frames from the external reference video

The second capability is preservation-oriented.
The first capability is the one needed to protect a renaming campaign.

## Suggested Automation Output Layout

Recommended directory structure:

```text
documentation/
  regression/
    baseline/
      java-current/
        manifest.csv
        frames/
    candidate/
      manifest.csv
      frames/
    compare/
      frame_metrics.csv
      summary.md
      visuals/
```

Recommended CI or job outputs:

- compile log
- capture manifest diff
- frame metrics CSV
- Markdown summary
- top N visual diffs

## Practical Rename Rules

To keep the campaign safe:

- never mix renaming and behavior changes in the same commit
- rename one subsystem at a time
- keep a legacy-name comment at class level for a while
- update reflection strings in the same patch
- commit after every successful compile and capture check

A useful temporary class header pattern is:

```java
// Legacy name: kmajmka
public class WatercubeScene extends Scene {
    ...
}
```

This preserves traceability against old reverse-engineering notes.

## Effort Estimate

For one engineer working carefully:

### First readability pass

- `3 to 6 days`

Expected output:

- all scene classes renamed
- scene lifecycle methods renamed
- main runtime and registries renamed
- core math, render, and loader classes renamed

### Shared-engine cleanup

- `4 to 8 days`

Expected output:

- major renderer, camera, and loader methods renamed
- most important shared fields renamed

### Deep per-scene cleanup

- `2 to 4 weeks`

Expected output:

- scene-private fields and locals renamed where justified
- comments added where names would still be speculative

## Bottom Line

Renaming a large share of the obfuscated Java source is realistic and worth doing.

The best target is not "rename everything at once".
The best target is:

- rename almost all top-level concepts first
- rename shared engine methods second
- treat scene-private internals as a slower, evidence-based pass

With the current capture system, compile gate, and manifest metadata, automatic regression tracking is practical.
The only notable missing piece is the actual comparison script that the workflow document already assumes.

So the recommended path is:

1. freeze a capture baseline
2. create a symbol map with confidence levels
3. rename by subsystem
4. run compile and capture comparison after every batch
5. use deeper scene-local cleanup only after the engine skeleton is readable
