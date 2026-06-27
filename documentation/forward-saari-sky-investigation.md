# Forward Saari Sky Investigation

## Goal

Determine why the `saari` sky/backdrop in the Java desktop reconstruction shows large triangle-shaped faceting, while the reference video looks much smoother.

This note tracks what is already proven, what has been ruled out, what was tested, and the current probe-driven next step.

---

## Current Conclusion

At this stage, the most likely explanation is:

- not a modern JDK hosting issue by itself
- not a mismatch in `saari` backdrop UV generation, camera projection, or visible triangle construction
- a real rasterizer reconstruction mismatch did exist in `kaajmma`, and has now been corrected
- after that correction, the original and desktop backdrop raster preview also match at the probe checkpoint
- the remaining mismatch versus the 1998 reference video is therefore less likely to come from the reconstructed Java source itself at this checkpoint

The strongest new evidence is the numeric probe:

- at `t = 144000 ms`
- with desktop mode `forward.saariBackdropUvMode=procedural`
- original and desktop builds produce exactly the same projected backdrop vertices and exactly the same visible backdrop triangles

That moved the main suspicion downstream of `kaaakma` and downstream of the scene/frustum stage, into the rasterizer.

---

## Why This Does Not Look Like A JDK API Problem

The sky is rendered by the original software renderer structure, not by a GPU-backed Java 2D texture mapper.

Relevant code paths:

- `java-desktop/src/main/java/kaaakma.java`
- `java-desktop/src/main/java/mmajmmk.java`
- `java-desktop/src/main/java/kaajmma.java`
- `java-desktop/src/main/java/mmaamma.java`

What matters:

- `kaaakma` loads `meshes/half8.igu` and `images/verax/tai1sp.jpg`.
- `mmajmmk.KKAmaJa(...)` computes per-vertex UVs procedurally.
- `kaajmma.MAjakKa(...)` then rasterizes triangles directly into the software framebuffer.
- `mmaamma` stores pixels in packed integer arrays and the host only presents the final image.

Because the visible artifacts follow triangle boundaries on the backdrop dome, they are much more consistent with UV/projection/rasterization differences than with an AWT hosting change.

That does not mathematically prove the JDK has zero impact, but it makes "modern JDK API change" a low-probability primary cause.

---

## What Has Already Been Proven

### 1. `kaaakma` backdrop setup matches the original bytecode closely

The original class really does:

- load `meshes/half8.igu` for `saari`
- scale the mesh to `10000`
- call `KKAmaJa(...)`
- then flip `u` with `u = 1 - u`

The reconstructed source follows that pattern.

Relevant files:

- `java-desktop/src/main/java/kaaakma.java`
- `reverse/cfr_single/kaaakma.java`
- `original/forward/kaaakma.class`

### 2. `mmajmmk.KKAmaJa(...)` also matches the original bytecode closely

The procedural mapping path was checked against `javap` output for the original class.

Important point:

- the current source already reproduces the original call pattern and general math flow
- so the sky issue is not explained by a simple "wrong method entirely" situation

Relevant files:

- `java-desktop/src/main/java/mmajmmk.java`
- `reverse/cfr_single/mmajmmk.java`
- `original/forward/mmajmmk.class`

### 3. The desktop-specific projective cleanup for materials `3` and `259` was not source-faithful

That cleanup had been added earlier in the desktop reconstruction to reduce seams, but it was an intentional divergence from the original affine rasterizer.

It has now been reverted.

Relevant file:

- `java-desktop/src/main/java/kaajmma.java`

Documentation updated:

- `java-desktop/README.md`
- `documentation/forward-java-desktop-reconstruction-process.md`

### 4. That `3` / `259` cleanup was probably not the main cause of the sky itself

The `half8.igu` backdrop faces stay on material `1` by default.

That means:

- the reverted `3` / `259` projective cleanup affected `saari` fidelity in general
- but it was not the direct explanation for the sky dome faceting

### 5. The source-faithful sky still looks wrong

After restoring the affine rasterizer, the sky remains visibly more faceted than the reference video.

This means:

- reverting the desktop cleanup was necessary for fidelity
- but it did not solve the sky discrepancy by itself

### 6. The numeric `saari` probe is now implemented and working

Implemented files:

- `tools/java-src/ForwardSaariProbe.java`
- `tools/compare_saari_probe.py`
- `probe_saari_sky_original.bat`
- `probe_saari_sky_java_desktop.bat`
- `compare_saari_sky_probe.bat`

Probe outputs:

- `documentation/reference-capture/saari-probe/original`
- `documentation/reference-capture/saari-probe/java-desktop`
- `documentation/reference-capture/saari-probe/compare`

### 7. At `t = 144000 ms`, backdrop geometry parity is exact

The probe and compare run shows:

- same backdrop vertex count: `145`
- same visible backdrop triangle count: `49`
- same projected vertex values
- same visible triangle values
- same UVs
- same clip flags

Practical conclusion:

- the first confirmed parity break is not in backdrop mesh construction
- not in `mmajmmk.KKAmaJa(...)`
- not in the frustum/projection stage for this checkpoint
- not in the visible-triangle list emitted by `mmajmmk.kkaMAJa(...)`

So the next likely divergence point is now the affine rasterizer path itself.

### 8. A bytecode-level rasterizer mismatch was found in `kaajmma`

The helper below had been reconstructed incorrectly:

- `java-desktop/src/main/java/kaajmma.java`
- method: `MajAkKa(float)`

The original bytecode does:

- `f2l`
- `l2i`

The reconstructed source had:

- `(int)f`

Those are not equivalent for oversized fixed-point values:

- `(int)f` uses Java float-to-int saturation semantics
- `(int)(long)f` preserves the original wraparound after the intermediate `long`

This helper is used to seed the affine fixed-point interpolants for:

- `u`
- `v`
- shade/depth-like per-pixel term

So this is a real rasterizer parity bug, not a cosmetic cleanup.

### 9. After the `MajAkKa(float)` correction, backdrop raster parity is exact at the probe checkpoint

The `saari` probe now also emits:

- `backdrop_raster_preview.png`

At `t = 144000 ms`:

- original probe preview and desktop probe preview are pixel-identical for the backdrop
- manual local verification reported `diff_pixels=0` over `131072` pixels

This means that, at least for the backdrop-only render at this checkpoint, the current desktop reconstruction now matches:

- original projected geometry
- original visible triangle list
- original rasterized backdrop pixels

---

## Experiments Already Run

### Experiment A: test alternate backdrop UV interpretations

A temporary diagnostic switch was added in `kaaakma`:

- property: `forward.saariBackdropUvMode`
- modes: `procedural`, `mesh`, `spherical`

Purpose:

- compare the current procedural UV path against two alternate interpretations without rewriting the scene

Tested around the reference checkpoint:

- `t = 144002 ms`

Observed result:

- `procedural`: still the least wrong, even though it remains too faceted
- `mesh`: significantly worse, with a mostly blown-out/incorrect sky
- `spherical`: also incorrect, with very obvious artificial structure

Practical conclusion:

- the current procedural mapping remains the best candidate
- replacing it with embedded mesh UVs or a simple spherical remap is not a faithful fix

### Experiment B: force projective rendering on material `1`

This was tested only as a diagnostic.

Result:

- it is not valid for material `1`
- the projective path expects palette/shade-lut style data that the truecolor sky path does not provide
- the run failed with a `NullPointerException`

Conclusion:

- there is no safe interpretation where "just make the sky projective too" becomes the right fix

---

## What Is Still Unclear

One of these is still likely true:

1. The original applet already had some faceting, but it was softened by capture conditions, scaling, or compression in the video.
2. A historical JVM/browser/runtime behavior differs from the modern JVM used for both `original/forward` and `java-desktop`.
3. The remaining mismatch sits outside the isolated backdrop probe, for example in full-scene composition or presentation rather than in the backdrop raster itself.

At the moment, option 1 or 2 is stronger than "desktop source reconstruction bug" for this specific checkpoint.

---

## Next Investigation Protocol

The next step is no longer "compare geometry numerically" because that step is now implemented and already narrows the issue.

### Step 1: keep the probe as the geometry gate

Before any new visual fix is attempted:

- run `probe_saari_sky_original.bat`
- run `probe_saari_sky_java_desktop.bat`
- run `compare_saari_sky_probe.bat`

If these results ever stop matching, a geometry-stage regression was introduced.

### Step 2: verify whether a full-scene preview still differs from the reference video

The backdrop-only probe is now source-faithful at the tested checkpoint.

The next useful comparison is therefore:

- full scene render at the same checkpoint
- against `documentation/reference-capture/reference/frames/ref_000036_t00144002.png`

If the backdrop still looks too faceted in the full scene despite backdrop raster parity, the remaining cause is likely:

- capture/presentation
- historical JVM precision
- or another non-backdrop rendering stage

### Step 3: only after a full-scene mismatch is isolated, change more code

No further "visual improvement" change should be treated as a faithful fix until one of the previous steps identifies the first actual divergence point.

---

## Reproduction Notes

Reference capture frame already available in the repository:

- `documentation/reference-capture/reference/frames/ref_000036_t00144002.png`

Java reconstruction capture used for comparison:

- `documentation/reference-capture/java/frames/frame_000036_t00144002_f039112.png`

Diagnostic capture command pattern:

```bat
set JAVA_TOOL_OPTIONS=-Dforward.saariBackdropUvMode=procedural
set CAPTURE_DIR=build\saari-sky-procedural
set CAPTURE_INTERVAL_MS=5000
set CAPTURE_START_MS=144000
set CAPTURE_STOP_MS=145000
set CAPTURE_LIMIT=1
set ENABLE_CAPTURE_EXIT=1
set ENABLE_NOSOUND=0
capture_forward_demo.bat
```

Available diagnostic modes:

```text
procedural
mesh
spherical
```

These modes exist only to support investigation. They are not alternate faithful render paths.

Numeric probe command pattern:

```bat
set SCENE_TIME_MS=144000
probe_saari_sky_original.bat
probe_saari_sky_java_desktop.bat
compare_saari_sky_probe.bat
```

Detailed probe workflow:

- `documentation/forward-saari-probe-workflow.md`

---

## Documentation Links

Related files:

- `documentation/forward-saari-finalization-roadmap.md`
- `documentation/forward-java-desktop-reconstruction-process.md`
- `documentation/forward-saari-probe-workflow.md`
- `documentation/forward-reference-capture-workflow.md`
- `java-desktop/README.md`

This file should be updated whenever:

- a new hypothesis is ruled out
- a new numeric dump is added
- a new source-faithful fix is proven
