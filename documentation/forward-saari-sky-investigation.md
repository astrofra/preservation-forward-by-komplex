# Forward Saari Sky Investigation

## Goal

Determine why the `saari` sky/backdrop in the Java desktop reconstruction shows large triangle-shaped faceting, while the reference video looks much smoother.

This note tracks what is already proven, what has been ruled out, what was tested, and the next comparison protocol.

---

## Current Conclusion

At this stage, the most likely explanation is:

- not a modern JDK hosting issue by itself
- not yet proven to be a single obvious decompilation mistake in `kaaakma`
- more likely a subtle reconstruction gap in math, projection, or software rasterization behavior

The evidence currently points much more strongly toward reconstruction/parity drift than toward `Applet` removal, `Panel` hosting, or modern AWT image presentation.

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

1. A subtle reconstruction difference exists in low-level vector math or rotation helpers.
2. A subtle reconstruction difference exists in affine interpolation or scan conversion.
3. The original applet already had some faceting, but it was softened by capture conditions and lower apparent sharpness in the video.
4. A precision-sensitive behavior changed between the 1998-era JVM/runtime and the current desktop run, even though the code structure is nominally the same.

At the moment, option 1 or 2 is the strongest working hypothesis.

---

## Next Investigation Protocol

The next step is to compare original bytecode behavior and reconstructed source behavior numerically, not only visually.

### Step 1: dump generated backdrop UVs

For a fixed subset of `half8.igu` vertices:

- original runtime: dump the UVs after `KKAmaJa(...)`
- reconstructed runtime: dump the UVs after `KKAmaJa(...)`

If the UVs already differ, the bug is upstream of rasterization.

### Step 2: dump projected screen coordinates for selected sky triangles

For a fixed `saari` frame near `t = 144002 ms`:

- record the three screen-space vertices for several backdrop triangles
- record their UVs and depth values

If UVs match but projected vertices differ, the problem is in transform/projection parity.

### Step 3: dump per-scanline interpolation state for one or two suspect triangles

For a chosen visible sky triangle:

- left/right x bounds
- initial `u`, `v`, shade terms
- per-scanline or per-span deltas

If geometry matches but span state diverges, the problem is in the affine rasterizer reconstruction.

### Step 4: only after the numeric comparison, change code

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

---

## Documentation Links

Related files:

- `documentation/forward-saari-finalization-roadmap.md`
- `documentation/forward-java-desktop-reconstruction-process.md`
- `documentation/forward-reference-capture-workflow.md`
- `java-desktop/README.md`

This file should be updated whenever:

- a new hypothesis is ruled out
- a new numeric dump is added
- a new source-faithful fix is proven
