# Forward Saari Probe Workflow

## Goal

Provide a repeatable numeric comparison path between:

- the original `forward` Java bytecode in `original/forward`
- the reconstructed desktop Java source in `java-desktop`

This probe is specific to the `saari` sky/backdrop investigation.

---

## Scripts

Repository root wrappers:

- `probe_saari_sky_original.bat`
- `probe_saari_sky_java_desktop.bat`
- `compare_saari_sky_probe.bat`

Helper sources:

- `tools/java-src/ForwardSaariProbe.java`
- `tools/compare_saari_probe.py`

---

## What The Probe Dumps

Each runtime writes a probe directory containing:

- `summary.txt`
- `camera_state.csv`
- `scenegraph.csv`
- `backdrop_vertices.csv`
- `backdrop_faces.csv`
- `backdrop_projected_vertices.csv`
- `backdrop_visible_triangles.csv`
- `backdrop_raster_preview.png`

The compare step writes:

- `comparison_summary.txt`
- `projected_vertex_diff.csv`
- `visible_triangle_diff.csv`

`comparison_summary.txt` now also reports whether:

- `backdrop_raster_preview.png` is present on both sides
- the PNG files are byte-identical

The screen-space values are exported in two forms:

- raw engine `16.16` fixed-point values
- pixel-space values derived by dividing by `65536`

The raster preview is a direct software-rendered backdrop-only PNG emitted by the probed runtime at the requested checkpoint.

---

## Default Output Locations

- original probe:
  - `documentation/reference-capture/saari-probe/original`
- desktop probe:
  - `documentation/reference-capture/saari-probe/java-desktop`
- comparison output:
  - `documentation/reference-capture/saari-probe/compare`

---

## Usage

### 1. Probe the original build

```bat
set SCENE_TIME_MS=144000
probe_saari_sky_original.bat
```

Notes:

- the wrapper runs the original runtime with `-Xverify:none`
- this is required because the shipped `forward.class` contains bytecode metadata that modern JVMs reject during normal verification
- default `JAVA_HEADLESS` is `false`, because `Applet` construction throws in headless mode

### 2. Probe the desktop reconstruction

```bat
set SCENE_TIME_MS=144000
set SAARI_UV_MODE=procedural
probe_saari_sky_java_desktop.bat
```

Notes:

- the wrapper recompiles `java-desktop/src/main/java`
- the helper is compiled separately against the desktop build
- default `JAVA_HEADLESS` is `true` for the desktop path

### 3. Compare the two dumps

```bat
compare_saari_sky_probe.bat
```

Optional variables:

- `PROBE_LEFT_DIR`
- `PROBE_RIGHT_DIR`
- `PROBE_COMPARE_OUTPUT_DIR`

---

## Supported Wrapper Variables

Shared:

- `SCENE_TIME_MS`
- `PROBE_OUTPUT_DIR`
- `PROBE_LABEL`
- `JAVA_HEADLESS`

Desktop-only:

- `SAARI_UV_MODE`

Available desktop UV diagnostics:

- `procedural`
- `mesh`
- `spherical`

---

## Current Confirmed Result

Reference checkpoint used during implementation:

- `SCENE_TIME_MS=144000`
- desktop UV mode: `procedural`

Observed result from `compare_saari_sky_probe.bat`:

- projected backdrop vertex count matches exactly: `145`
- visible backdrop triangle count matches exactly: `49`
- all projected vertex fields match exactly
- all visible triangle fields match exactly
- no clip-flag mismatch
- no UV mismatch

Additional rasterizer check performed after the `kaajmma.MajAkKa(float)` correction:

- `backdrop_raster_preview.png` from `original` and `java-desktop` are pixel-identical at this checkpoint

This means the current `saari` discrepancy is not in:

- backdrop mesh loading
- backdrop procedural UV generation
- camera/frustum update for this checkpoint
- projected vertex generation
- visible triangle list construction

For this checkpoint, the first proven parity break must therefore be downstream of visible triangle generation.

The highest-priority next instrumentation target is now:

- full-scene checkpoint comparison against the video reference

The rasterizer-specific decompilation mismatch that was found and fixed is:

- `java-desktop/src/main/java/kaajmma.java`
- helper `MajAkKa(float)`
- corrected from `(int)f` to `(int)(long)f` to match the original bytecode `f2l; l2i`

---

## Recommended Next Step

Add a second probe layer that dumps, for one or two selected `saari` backdrop triangles:

- triangle ID / source face ID
- start and end x for each scanline
- initial interpolants
- per-scanline deltas
- sampled texture coordinates after fixed-point conversion

That will tell whether the faceting difference appears:

- before span emission
- during affine interpolation
- or during final framebuffer/capture presentation
