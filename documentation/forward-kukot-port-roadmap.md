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
  - shared `ASE` rotation playback now uses a cyclic squad-style quaternion tangent interpolation, reducing the earlier pose drift left by plain `slerp`
  - `kukot` mesh deformation equivalent to Java `jAkKAma = 2`
  - material `3` env lookup closer to Java: indexed `envplane.gif` indirection into the generated RGB gradient, shaded by the original affine screen-space depth row path
  - env-map vectors now come from mesh normals, matching the Java `Triangle -> MeshObject.KKAMaja()` path instead of reusing normalized vertex positions
  - flare sprites now follow the Java `mAjAkka = 1024` contract more closely: additive nearest-neighbor scaled blit with depth-sized quads rather than the earlier oversized radial sampler
  - `flare1.jpg` is now explicitly unpacked from the original Java `<<20/<<10` RGB layout before C++ additive blending, which removes the false “overflow” ring/cycle artifact seen on close bright flares
  - the two Java blur passes are now separated correctly: `RgbSurface.aMajAKK(0.875f)` is ported as the in-frame left-to-right VHS-like smear, and `RgbSurface.AmajakK()` remains the end-of-frame temporal ghosting pass
  - the blur/ghosting passes now also run with Java-like packed-color math instead of the earlier RGB approximation, which reduces some of the excess glow in `kukot`
  - material `3` rasterization now keeps env UVs and shade affine in screen space, matching the original `TexturedTriangleRasterizer` path rather than using perspective-correct interpolation

Comparison note:
- the first Java-vs-C++ comparison that triggered this pass was not time-aligned
- Java reference `frame_000044_t00176006_f045885.png` is at `demo_time=176.006s`
- the comparable C++ full export area is around `frame_008800.tga` / `frame_008801.tga`, not `frame_009011.png`

Known fidelity gaps:
- camera positions still use a looped spline approximation rather than a full direct port of the Java `SplineTrack` basis
- object rotations are closer after the shared squad-style tangent pass, but are still an approximation of the Java quaternion spline machinery rather than a byte-for-byte port
- the particle cloud is source-shaped in placement, scale, and additive sprite blit, but still bypasses some of the original `ParticleCloudMesh` / `SurfacePresenter` batching details
- triangle submission still skips Java near-plane clipping and some material/compositing nuances from `SceneRenderer` / `MeshObject`
- current composition is materially closer than the first pass, but still diverges from the Java capture in silhouette smoothness, exact flare occlusion ordering on edge cases, and highlight spread/intensity tuning
- some side-by-side playback comparisons are still offset by roughly `0.5s`, so visual judgments should continue to prioritize shot content and manifest alignment over raw video timestamp

## Validated Aesthetic Override

`kukot` now carries one explicit local exception to the default source-faithful policy.

What the Java reconstruction says:
- `MeshObject.KKAmaja()` deforms the mesh positions first for `jAkKAma = 2`
- `MeshObject.KKAMaja()` then computes env-map UVs from the stored vertex normals after matrix transform
- `MeshObject.kKamaja()` keeps the original screen-space winding test for face submission
- in other words, the repaired Java source does not contain an explicit "flip normals" or "invert backface culling" step for this scene

What visual review says:
- in side-by-side capture review, the current C++ `kukot` bodies can still read as visually "inside out" or "in relief" compared with the Java output
- side-by-side review after the latest patch showed a clear improvement in body relief and overall shot reading even with an estimated timing mismatch of about `0.5s`
- the improvement was strong enough to justify keeping the deviation rather than treating it as a temporary probe
- the retained override is intentionally narrow: flipped env-map normals plus reversed torsion direction, while backface culling stays on the Java-side contract

Preservation rule for this case:
- source-faithful behavior remains the default policy for the project overall
- `kukot` is now a documented exception on two points only: env-map normal sign and torsion rotation sense
- this exception is justified by observed aesthetic fidelity, not by a claim that the repaired Java source literally does the same thing
- the choice must remain explicit, local to `kukot`, and easy to revisit in later comparisons
- if a later renderer-level diagnosis reproduces the same visual result without semantic deviation, prefer that explanation and remove the override

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
