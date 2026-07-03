# Forward Kukot Port Roadmap

## Goal

Port `kukot` (`kajjkka` / `KukotScene`) to the offline C++ exporter as a source-shaped scene, with the original `ASE` camera/object motion, `jarnomix.xm` timing window, env-mapped meshes, flare cloud, and scripted flash overlay preserved as evidence-backed behavior rather than replaced by a generic placeholder.

Primary Java source:
- `java-desktop/src/main/java/KukotScene.java`

Supporting reconstruction/runtime sources:
- `java-desktop/src/main/java/AseSceneLoader.java`
- `java-desktop/src/main/java/SceneRenderer.java`
- `java-desktop/src/main/java/MeshObject.java`
- `java-desktop/src/main/java/ParticleCloudMesh.java`
- `java-desktop/src/main/java/FlashNoiseOverlay.java`
- `documentation/forward-global-script-timeline.md`

## Current C++ Status

Already present:
- standalone `--sequence kukot` support in `forward-export`
- native `jarnomix.xm` audio sliced from the real handoff point `0x0700`
- first direct-asset load path for:
  - `original/forward/asses/under1.ase`
  - `original/forward/images/envplane.gif`
  - `original/forward/images/flare1.jpg`
- tracked camera playback from the original `ASE` scene
- first env-mapped triangle pass for the three `under1.ase` objects
- static flare-cloud pass centered around the original Java placement
- tiled procedural background noise and message-driven flash overlay
- full-wrapper integration through `cpp-offline/scripts/export_intro_full.bat`
- Java-shaped fixes now landed for the first fidelity pass:
  - looped spline-style sampling for `ASE` position tracks in `scene3d_shared`
  - accumulated quaternion playback for object rotation tracks instead of treating `*CONTROL_ROT_SAMPLE` values as absolute
  - `kukot` mesh deformation equivalent to Java `jAkKAma = 2`
  - material `3` env lookup closer to Java: indexed `envplane.gif` indirection into the generated RGB gradient, shaded by perspective-correct depth fraction

Comparison note:
- the first Java-vs-C++ comparison that triggered this pass was not time-aligned
- Java reference `frame_000044_t00176006_f045885.png` is at `demo_time=176.006s`
- the comparable C++ full export area is around `frame_008800.tga` / `frame_008801.tga`, not `frame_009011.png`

Known fidelity gaps:
- camera and object positions now use a looped spline approximation, but not the full Java `SplineTrack` tangent/quaternion machinery yet
- object rotations are now accumulated correctly, but still use direct quaternion `slerp` rather than the full Java quaternion spline path
- the particle cloud is source-shaped in placement and scale, but still rendered through a simplified sprite pass rather than the original `ParticleCloudMesh` / `SurfacePresenter` contract
- triangle submission still skips Java near-plane clipping and some material/compositing nuances from `SceneRenderer` / `MeshObject`
- current composition is materially closer than the first pass, but still diverges from the Java capture in silhouette smoothness, flare occlusion, and white-hot highlight balance

## Refactor Verdict

Light refactor: yes.

What was worth extracting now:
- shared `ASE` block parsing
- shared mesh/track structures
- shared looped position-track sampling
- shared rotation-delta accumulation and orientation-track sampling

Implemented in:
- `cpp-offline/src/scenes/scene3d_shared.h`
- `cpp-offline/src/scenes/scene3d_shared.cpp`

Why this was the right cutoff:
- both `saari` and `kukot` need the same source ingest layer
- that code is structurally stable and scene-agnostic
- extracting more than that right now would force `saari`'s terrain/reflection-specific raster contracts into an artificial common layer

What was deliberately not refactored yet:
- triangle rasterizers
- depth-sort/compositing policy
- scene-local material semantics
- terrain/water/reflection logic

Reason:
- `saari` still carries effect-specific contracts that are not valid for `kukot`
- `kukot` currently needs iteration speed more than a prematurely unified renderer

## Immediate Next Steps

1. Port or approximate the remaining Java `SplineTrack` quaternion tangent path if current `slerp` still leaves visible pose drift on later `kukot` shots.
2. Add Java-style near-plane clipping for `kukot` triangles before spending more time on color tuning; close shots still expose the current drop-triangle shortcut.
3. Compare representative Java capture frames around `0x0900`, `0x0B00`, and `0x0C40` against the updated C++ output to tune flare occlusion and highlight spread.
4. Reassess a deeper 3D renderer refactor only after a third `ASE`-driven scene lands and shared contracts are no longer speculative.
