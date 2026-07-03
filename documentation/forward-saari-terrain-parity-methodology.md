# Forward Saari Terrain Parity Methodology

## Goal

Record the method used to debug and correct the recent native `saari` terrain regression in `cpp-offline`.

This is meant as a reusable parity-debugging reference, not just a changelog for one fix set.

---

## Why This Note Exists

The visible symptom looked simple:

- mountain looked broken
- sea/reflection polygons pierced the foreground
- the result suggested a possible normal flip or axis inversion

The real cause was not one isolated "wrong normal" bug.

It was a stack of Java-specific renderer contracts that had drifted in the C++ port:

- terrain sample semantics
- terrain face-test signs
- reflection gating
- draw ordering
- raster interpolation mode
- camera setup order

The practical lesson is:

- do not trust the first 3D-engine intuition
- inspect the Java scene-specific renderer before applying visual tweaks

---

## Working Reference

Use the frozen Java baseline as the behavioral oracle:

- `documentation/reference-capture/baseline-java-2026-06-29/`

Useful Saari checkpoints from that baseline:

- `frame_000027_t00108000_f029987.png`
- `frame_000032_t00128003_f034583.png`
- `frame_000036_t00144004_f038159.png`
- `frame_000037_t00148001_f039097.png`

For merged native exports, recover the scene-local offset first.

Example:

- `cpp-offline/output-full-current/log.txt` recorded `intro_frames=4940`
- therefore local `saari` frame = merged full frame - `4940`
- `frame_006365` -> local `frame_001425`
- `frame_007288` -> local `frame_002348`

This avoids debugging the right symptom on the wrong frame.

---

## Recommended Debug Loop

### 1. Reduce the problem to the scene-local export

Prefer a short dedicated export over the full wrapper:

```powershell
cpp-offline/build/Release/forward-export.exe --sequence saari --frames 2350 --output cpp-offline/output-saari-regression-check --no-log
```

Then inspect only the few matching checkpoints.

This shortens iteration time and keeps frame numbering stable.

### 2. Inspect Java invariants before editing C++

For Saari, the key source files were:

- `java-desktop/src/main/java/SaariScene.java`
- `java-desktop/src/main/java/SaariTerrainMesh.java`
- `java-desktop/src/main/java/AseSceneLoader.java`
- `java-desktop/src/main/java/SceneRenderer.java`
- `java-desktop/src/main/java/TexturedTriangleRasterizer.java`

The useful rule was:

- derive renderer behavior from Java code first
- use images only to confirm that the inferred behavior explains the regression

### 3. Classify each suspected mismatch

Do not lump everything under "normals" or "UVs".

For this pass, the productive buckets were:

- camera/time-domain behavior
- terrain sample generation
- visibility and face-test logic
- reflection composition rules
- reflected concave-mesh self-overlap
- draw-order model
- raster interpolation mode

### 4. Apply one Java contract at a time

Each correction should map to a concrete Java behavior.

The corrections that mattered here were:

- camera orientation comes from position + target track; camera rotation track is not the active orientation source in this scene
- camera `z` clamp happens after camera basis/orientation is built
- in-bounds terrain heights use the `-16` bias but clamp to non-negative land
- out-of-heightmap samples fall back to `-0.001f`, which is what creates flat water outside the island
- terrain reflection uses the terrain texture with the black ramp, not the water texture
- additive reflection should consume its water-mask hit once
- Java material `259` stays additive for reflected meshes; changing it to opaque would hide the real parity bug instead of matching the source
- reflected `klunssi` visibility must follow the clone's mirrored object transform; recomputing a world-space face normal after the Z mirror can flip concave face selection and expose interior lobes that Java culls
- reflected `klunssi` env mapping also differs from the main mesh: the Java reflection is a `MeshObject` clone, and that clone does not copy the original `JAKkama` extra X-axis env-map tweak
- Saari terrain/env primitives behave like a shared depth-sorted batch, not a modern z-buffered scene
- Saari materials `3` and `259` are affine here, not projective
- the two half-triangles of each terrain quad use explicit slope-sign formulas; translating them as a generic face-normal test is error-prone
- Java does not appear to draw a separate "undersea horizon" mask; if the sky leaks through near the sea horizon, first audit water-patch coverage and backdrop mapping before suspecting postFX

### 5. Re-render and reclassify the remaining drift

After the terrain shards disappeared, the remaining mismatch class changed:

- no longer a terrain-topology or face-selection problem
- now more likely camera/timing/framing drift
- or a remaining backdrop / water-footprint coverage mismatch once the core terrain topology is already correct

That reclassification is important.

It prevents continuing to "fix geometry" after the geometry class of bug is already solved.

---

## Proven Methodological Takeaways

### 1. Specialized software scenes must be ported behavior-first

Saari is not "a heightmap plus some env-mapped meshes".

It is a scene with a custom renderer contract.

Port the contract, not the visual approximation.

### 2. Scene-local renderer semantics matter more than generic 3D intuition

The final breakthrough came from Java-specific details:

- average-depth sorting
- affine rasterization
- one-shot reflection masking
- mirrored reflected-face visibility for concave meshes
- clone-specific env-map state on the reflected `klunssi`
- terrain quad slope tests

Those are easy to miss if the port is treated like a generic mesh renderer.

### 3. A frozen baseline plus scene-local frame mapping is essential

Two checks made this tractable:

- fixed Java baseline frames
- explicit merged-frame to local-scene-frame conversion

Without that mapping, visual comparisons become noisy and ambiguous.

### 4. "Looks like normals" is not a diagnosis

In this case, the apparent normal inversion was mostly an emergent symptom of:

- wrong terrain visibility
- wrong ordering
- wrong interpolation mode
- or, later in the pass, wrong mirrored-face selection on the reflected `klunssi` clone rather than a truly wrong blend mode

Treat the first visual guess as a hypothesis only.

---

## Files To Revisit First Next Time

If Saari regresses again, inspect these native and Java files first:

- `cpp-offline/src/scenes/saari_scene.cpp`
- `java-desktop/src/main/java/SaariScene.java`
- `java-desktop/src/main/java/SaariTerrainMesh.java`
- `java-desktop/src/main/java/SceneRenderer.java`
- `java-desktop/src/main/java/TexturedTriangleRasterizer.java`

If the symptom again resembles broken land/sea/reflection visibility, re-check in this order:

1. frame mapping against the frozen baseline
2. terrain sample semantics
3. terrain face-test signs
4. primitive ordering
5. affine vs projective raster mode
6. camera order and timing

---

## Current Status After This Pass

This methodology produced a much closer native Saari terrain result.

Most notably:

- large broken mountain shards disappeared
- land/sea separation became coherent again
- reflected `klunssi` now behaves more like a single front-most mirrored layer instead of accumulating obvious interior concave lobes
- reflected `klunssi` shading now stays closer to Java because the mirrored clone no longer reuses the original mesh's extra env-map X tweak
- the remaining difference is now better described as camera/framing parity work than as terrain corruption

That is the main reason to keep this note:

- it captures how to move from a misleading visual symptom to a defensible renderer-level diagnosis
