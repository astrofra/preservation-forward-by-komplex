# Forward C++11 Offline Scaffold

This directory contains the first usable milestone of the `documentation/forward-cpp11-subset-offline-port-feasibility-study.md` plan:

- restricted `C++11`
- headless CLI exporter
- deterministic `50 fps` / `22050 Hz` offline timeline
- Java-style intro script player for `mute95 -> domina -> filmbox`
- first autonomous `saari` 3D pass with script-row shock events
- `saari-gsplat`: static PNG / COLMAP capture on an enclosing hemisphere, with a meditate focus
- `feta-gsplat`: constant-radius PNG / COLMAP sphere, frozen fetus and particles, stationary native yellow halo
- first autonomous `kukot` 3D pass with sliced `jarnomix.xm` playback from `0x0700`
- first autonomous `maku` terrain pass with sliced `jarnomix.xm` playback from `0x0D00`
- first autonomous `watercube` mixed 3D / packed-surface pass with sliced `jarnomix.xm` playback from `0x1000`
- direct loading of original assets from `original/forward`
- native `512x256` uncompressed `TGA` frames
- one stereo `16-bit PCM` `WAV`
- `manifest.csv` kept close to the Java capture format
- `ffmpeg` wrappers for muxing

The default export path now runs on an audio-sample master clock:

- `--sequence intro` is the default
- `--intro-frames-per-row` and `--intro-rows-per-order` remain available as legacy wrapper hints, but they no longer drive scene/script timing
- `--sequence saari` exports the current direct-asset `saari` 3D pass with row-driven `suh0` / `suh` shock events
- `--sequence kukot` exports the current direct-asset `kukot` pass, starting from `jarnomix.xm` song position `0x0700`
- `--sequence maku` exports the current direct-asset `maku` terrain flythrough, starting from `jarnomix.xm` song position `0x0D00`
- `--sequence watercube` exports the current direct-asset `watercube` mixed 3D / packed-surface pass, starting from `jarnomix.xm` song position `0x1000`
- `--sequence bootstrap` keeps the older placeholder scene available for quick pipeline checks

Current limitation:

- `mute95` and `domina` are now structured as real C++ scene/routine ports with the Java message names and timing flow
- `mute95` now loads the original JPEG/GIF assets directly at runtime through vendored `stb_image` plus native GIF palette handling
- the current `mute95` render is already very close to the Java reference capture, with remaining drift concentrated around the central blue halo / horizontal band during the title phase
- `mute95` now keeps the blurred background cloud on a stable intensity envelope instead of letting the C++ port re-feed it into a late-sequence glow runaway; this matches the current Java baseline and the 2026 Power Mac G5 ground-truth screenshots more closely
- `domina` now loads `images/phorward.gif` directly as an indexed GIF and follows the Java `512x3840 -> 512x256` frame-strip scroll path
- `domina` late-frame comparisons are still influenced by the synthetic song-position transport, so the remaining drift there is not yet a pure scene-renderer verdict
- `intro` now renders native `kuninga.xm` audio and `saari` now renders native `jarnomix.xm` audio directly in C++, with no Java intermediation
- `saari` now loads `tai1sp.jpg`, `saari.gif`, `envi_klu.gif`, `saarih15.gif`, and `asses/alku6.ase` directly in C++ for a source-shaped terrain/object/reflection pass with deterministic `suh0` / `suh` shock handling
- the current `saari` port now mirrors the key Java trick: a camera-local terrain patch whose out-of-heightmap samples become flat water, with palette-masked additive reflections and no explicit `meditate` mirror copy
- `saari` now also follows the Java terrain visibility path more closely: heightmap samples clamp to non-negative land, terrain/env materials `3` and `259` are rasterized through a shared depth-sorted primitive batch, and the terrain quad face test now matches the original two-triangle slope logic
- `klunssi` reflection parity is source-driven here: Java keeps material `259` additive, but only on a one-shot reflective water mask; its mirrored concave face selection must follow the clone's mirrored object transform rather than a recomputed post-mirror world-space cross product; and the reflected clone does not inherit the original `klunssi` env-map `JAKkama` X-axis tweak
- `saari` is still not at final camera parity with Java, but the terrain/material path is now substantially closer to the original renderer than the earlier background-water approximation
- `kukot` now loads `asses/under1.ase`, `images/envplane.gif`, and `images/flare1.jpg` directly in C++, then renders a substantially closer source-shaped pass with cyclic squad-style quaternion interpolation closer to the Java `SplineTrack` path, Java-style `jAkKAma = 2` mesh deformation, normal-driven indexed-env material `3` lookup with affine screen-space interpolation like the original rasterizer, depth-sized additive flare blits, tiled noise background, scripted flash overlays, a Java-style left-to-right horizontal smear (`RgbSurface.aMajAKK(0.875f)`), and separate Java-style temporal frame ghosting (`RgbSurface.AmajakK()`) using packed-color math closer to the original surface code
- `kukot` flare rendering now also normalizes `flare1.jpg` from the original Java `<<20/<<10` packed RGB layout into standard `0xRRGGBB` before sprite blending; the earlier close-shot ring/cycle artifact was a packing mismatch, not a true additive overflow
- the current `kukot` port uses the real `jarnomix.xm` song-position slice from `0x0700`, so the standalone sequence and the full wrapper no longer restart the module from the beginning when entering that scene
- `kukot` keeps one documented local aesthetic override bundle: the repaired Java source does not explicitly flip normals or culling, yet side-by-side review converged on a substantially closer result when the C++ port reverses torsion direction, inverts body normals and culling winding, and remaps the env-map projection basis to `u <- 0.5 * (1 - z)`, `v <- 0.5 * (1 - y)`
- `maku` now loads `images/scape/loopk40.gif`, `images/scape/loopa2.gif`, and the camera tracks from `asses/vuori5.ase` directly in C++, then renders a first source-shaped repeating canyon flythrough with Java-like `go` / `speed` script messages, `suh` shock lines, rolling camera toggle support, default frame averaging, and the later `ksor` invert/smear feedback burst
- process note: the baseline `maku` conversion itself was completed in a single prompt from the current exporter state, and was materially more direct than the previous scene-port attempts; the remaining work is now fidelity tightening rather than first-pass structural translation
- the current `maku` pass is already in the right visual family against the frozen Java captures, but the exact timing of the washed-out/fog-heavy states and the later feedback cadence still need tightening
- `watercube` now loads `asses/nosto3.ase`, `images/1.jpg`, `images/txt1.jpg`, `images/reunus2.jpg`, `images/env3.jpg`, `images/rinku2.jpg`, `images/riple2.jpg`, and `meshes/kluns1.igu` / `meshes/kluns2.igu` directly in C++, then reproduces the native ripple ping-pong buffers, right-side panel, giant text overlay, and scripted `pum` / `rok` / `suh*` / `tex*` message flow
- the current `watercube` pass is already close to the frozen Java captures in broad composition, with the main remaining drift concentrated around env-mesh lighting and deeper Java face-mode `49` parity
- a light shared refactor is now in place for `ASE`/track parsing (`src/scenes/scene3d_shared.*`), reused by both `saari` and `kukot`; the rasterizers remain separate because `saari` still carries scene-specific terrain/reflection contracts that would make a broader 3D unification premature
- Java-based normalization helpers may still exist for validation, but they are not part of the exporter runtime path

## Build

From the repository root:

```powershell
cmake -S cpp-offline -B cpp-offline/build
cmake --build cpp-offline/build --config Release
```

On macOS, Apple Command Line Tools are sufficient:

```bash
xcode-select --install
cmake -S cpp-offline -B cpp-offline/build
cmake --build cpp-offline/build
```

## Run

### Saari Gaussian Splatting capture

Run from the repository root after building. Windows / Visual Studio:

```powershell
cpp-offline/build/Release/forward-export.exe --sequence saari-gsplat --output cpp-offline/output-saari-gsplat
```

On a single-configuration CMake build, use `cpp-offline/build/forward-export`.
The output directory must not already exist. The entire export runs in C++11;
Python, ffmpeg, COLMAP and Postshot are not runtime dependencies.

Defaults: **300 PNG images at 1024x768**, frozen Saari local time **30 s**.
The camera moves on a hemisphere enclosing the land and frozen objects, at least
2 world units above the sea plane. Most views cover the island; a connecting pass
and a narrower 35-degree field of view cover meditate. The general pass uses the
original 1.4-radian horizontal field of view. Klunssi's translation, rotation and
reflection are frozen together; scripted shocks are disabled. Textures, fog and
reflection composition retain their original behavior.

| Option | Meaning |
| --- | --- |
| `--frames 300` | Total generated views, including held-out validation images (12..5000) |
| `--width 1024 --height 768` | Actual raster resolution, not image upscaling (16..4096 each) |
| `--gsplat-time 30` | Frozen scene-local time in seconds |
| `--gsplat-radius 0` | Auto-enclose scene geometry; positive values override the radius and must enclose the scene |
| `--gsplat-validation-every 10` | Keep every tenth view outside training; `0` uses all views for training |
| `--gsplat-camera-path path.csv` | Replay an edited `camera_path.csv`; row count replaces `--frames` |

The export writes:

- `images/`: training PNGs (270 with the defaults).
- `sparse/cameras.txt`, `images.txt`, `points3D.txt`: COLMAP text model, with exact
  intrinsics/poses and colored surface samples observed in at least two training views.
- `validation/images/` and `validation/sparse/`: held-out views and their model;
  their pixels do not contribute to training-point colors or point selection.
- `camera_path.csv`: native positions, targets, horizontal FOV in degrees and view groups.
- `manifest.csv`: image IDs, paths, groups, split, frozen time and camera IDs.
- `capture.json`: completed-export marker, enclosure parameters, counts, coordinate
  conversion and source revision at CMake configure time (including a dirty marker).
- `IMPORT.txt`: import instructions.

For Postshot, import **only `images/` and the three files in `sparse/` together**.
Keep `validation/` out of training; do not import the whole output root.
COLMAP coordinates reflect native world X (`x_export=-x_native`); world Z remains
up. Quaternions and translations are world-to-camera transforms. The sparse cloud
uses the same conversion. The optional owner buffer follows actual painter and
reflection-compositing order; points on hidden surfaces, the sky and composited
reflection pixels are excluded. Point colors average training observations.

CSV replay requires the same frozen time and resolution to reproduce images.
Camera positions must remain on the same hemisphere around the computed center;
with radius 0, the first CSV position determines its radius. CSV columns are
`px,py,pz,tx,ty,tz,hfov_degrees,group`; valid groups are `island`, `bridge`, `meditate`
and `custom`. The loader rejects underwater positions, vertical look directions,
nonfinite numbers, FOVs outside 10..120 degrees and radii that do not enclose the scene.

Validation:

```powershell
ctest --test-dir cpp-offline/build -C Release --output-on-failure
```

CTest uses an optional Python 3 **test-only** script with no third-party modules.
It checks PNG chunks/decompression, COLMAP reprojection, native camera conventions,
reciprocal point tracks, held-out isolation, exact CSV replay, static-scene
determinism and invalid inputs. Before/after captures of 50 original Saari frames
at one-second intervals, plus their WAV and manifest, matched byte for byte on
2026-09-24. Visual checks covered island and meditate views.

The installed Postshot v1.0.116 CLI detected the 33 training images in the smoke
dataset, then required a Studio license. A completed Postshot import/training run
has **not** been validated. Use the GUI to check the generated model. Exact poses
do not remove the original affine-texture and reflection inconsistencies.

Design and preservation rationale:
[Saari capture approach](../documentation/forward-saari-gsplat-capture-approach.md).

### Maku Gaussian Splatting capture

```powershell
cpp-offline/build/Release/forward-export.exe --sequence maku-gsplat --output cpp-offline/output-maku-gsplat-grid
```

Defaults: **300 views per path plus 1326 grid views, 1926 PNGs total, at 1024x512**, horizontal FOV **80 degrees** (the demo
uses 1.2 radians, about 68.75 degrees). The original 2:1 aspect ratio is retained.
`--gsplat-fov 90` changes the horizontal FOV; valid values are 10..120 degrees.
`--frames` sets the number of views **per path** (12..5000); dimensions work as for Saari.
Use a new output directory for each dataset.

The original path is followed by a second pass raised by **H/4**, where H is the
terrain's vertical extent, measured from the used heightmap samples with the
renderer's height scale. For the original assets H is about 351.14 units, so the
offset is **87.785 units**. Both camera and target move upward; orientation, FOV,
sampling times and fog are preserved. Both paths share `images/` and one COLMAP
model in `sparse/`, with unique image IDs and a common world/point cloud.
`--gsplat-height-fraction 0` disables the raised path; the default is `0.25`
and the accepted range is 0..1. `--gsplat-validation-every` holds out the same
sample indices in each pass.

The third pass is a **13 x 6 overhead grid**. Its XY bounds are the nominal
camera-position bounds enlarged to **150% in each dimension**, around the same
center (25% of the original width added on each side). Bounds are sampled at a
fixed 50 Hz, including both sides of script cuts and the scene endpoint. They
do not depend on `--frames` or the raised path. The original terrain repeats by
tiles, so the camera path defines the finite capture area.

Grid stations include the rectangle edges, with spacing no greater than **75
world units**. All stations sit at **Z = 505.6125**, which is the terrain maximum
plus H/8 (about 43.8925 units). Each station provides 17 views: eight azimuths
spaced by 45 degrees at 30 degrees downward, eight at 60 degrees downward, and
one straight down. Rows traverse in alternating directions. Original fog and the
200-unit seed-depth cutoff remain active; valleys may still be obscured.

| Option | Meaning |
| --- | --- |
| `--gsplat-grid-scale 1.5` | Nominal camera XY bounds scale, 1..3; `0` disables the grid |
| `--gsplat-grid-spacing 75` | Maximum station spacing in world units, 10..1000 |
| `--gsplat-grid-clearance 0` | Height above terrain maximum, 0..1000; `0` chooses H/8 |

The grid adds views independently of `--frames`; more than 10000 grid views is
rejected before creating output. To reproduce the earlier two-path export, use
`--gsplat-grid-scale 0`. To export only the nominal path, also use
`--gsplat-height-fraction 0`. Holdout indices restart at the beginning of the grid.

The exporter samples the **whole original Maku camera sequence**, from XM position
`0x0D00` inclusive to `0x1000` exclusive (about 24.407 seconds). It retains the
`vuori5.ase` position/target tracks and their interpolation, plus the script's five
`go`/`speed` segments, including backwards motion and cuts. Increasing `--frames`
increases sampling density without changing the duration or the path. There is
no hemisphere pass. The fixed 22050 Hz XM clock resolves the original script
timing without writing audio; `--fps` and `--sample-rate` do not affect this mode.

Fog and terrain materials remain unchanged. Temporal averaging, `ksor` feedback
and shocks are disabled for independent views: each PNG corresponds to its
exported pose. The repeating terrain uses distinct world-space points per tile.
Sparse seeds follow the renderer's final triangle visibility and need at least
two training observations; points at camera depth 200 or farther are excluded.
Fog and the original affine texture mapping can still limit reconstruction quality.

Output and Postshot import are the same as for Saari: **import `images/` together
with `sparse/cameras.txt`, `sparse/images.txt` and `sparse/points3D.txt`**. There are
1734 training views and 192 held-out views by default. `validation/` stays separate.
The shared COLMAP writer exports exact intrinsics, world-to-camera poses and the
same reflected-X coordinate convention. `camera_path.csv` retains Saari's first
eight columns, followed by `scene_time_seconds`, `track_time_seconds` and
`roll_radians` (zero), `pass` (`original`, `raised` or `grid`) and `height_offset`
in native world units, then `grid_x`, `grid_y`, `azimuth_degrees` and
`downward_degrees`. Grid views have static scene time 0 and empty `track_time_seconds`
and `height_offset`; grid-specific fields are empty for path views. This CSV documents the capture;
CSV replay and frozen-time/radius options are supported by Saari and Feta, not Maku.
`manifest.csv` associates every pose with its PNG. `capture.json` records FOV,
duration, terrain height bounds, offset, passes, split, point counts, original
script segments and completion status. Its `grid` section records the camera
bounds, scaled footprint, altitude, spacing, dimensions and counts, including
points shared with the paths and training grid views with no retained points.

The Maku CTest checks source ASE positions/targets, script timing and cuts, PNGs,
intrinsics, camera handedness, reprojections, reciprocal point tracks, validation
isolation, sampling independence, vertical translation with unchanged orientation,
shared tracks between passes, original-pass parity, FOV overrides and invalid input. A native Maku
before/after comparison also covers 40 frames at one-second intervals, WAV and
manifest. The grid CTest also checks bounds against the ASE, the 150% scale,
station coverage, all 17 directions including nadir, shared tracks, exact
path-image parity when enabling the grid, and grid invariance to temporal
sampling and raised-path settings. See [Maku capture notes](../documentation/forward-maku-gsplat-capture.md).

### Feta Gaussian Splatting capture

```powershell
cpp-offline/build/Release/forward-export.exe --sequence feta-gsplat --output cpp-offline/output-feta-gsplat-sphere-halo
```

Defaults: **300 PNG views at 1024x768**, horizontal FOV **80 degrees**, frozen
scene-local time **0 s**. The camera targets the center of the fetus mesh's bounds
and stays on a **constant-radius sphere**, about **15.852 world units** by default.
Automatic framing includes the fetus and particles using both horizontal and
vertical FOV. The path makes 10.5 turns with three elevation cycles between the
two hemispheres, covering the top, sides and bottom. There is no sea-plane constraint.

For more regular coverage, select **octahedral sampling**. This maps square-grid
cell centers to the sphere using the supplied `octDecode` algorithm. The frame
count must be a perfect square; without `--frames`, this mode uses **324 views
(18x18)**. It requires a constant radius. The mapping improves this scene's
coverage over the winding orbit, but is not an equal-area spherical mapping.

The following export keeps the halo, raises resolution to **1366x1024**, and
slightly varies the look target while retaining the octahedral camera positions:

```powershell
cpp-offline/build/Release/forward-export.exe --sequence feta-gsplat --gsplat-sampling octahedral --gsplat-target-offset 0.15 --gsplat-radius 15.852119 --width 1366 --height 1024 --output cpp-offline/output-feta-gsplat-octahedral-target015-h1024
```

`--gsplat-target-offset D` distributes targets over a disk of radius D world
units around the mesh center, perpendicular to each center-looking camera.
A golden-angle sequence with square-root radial spacing distributes disk area.
With D=0.15, the aim changes by at most about **0.54 degrees**. Camera origins
and their distance to the mesh center stay unchanged: this varies framing;
parallax comes from the different sphere positions. Omit this option for centered
targets. Both modes default to zero offset. The separate experimental
`--gsplat-lateral-offset D` moves camera origins tangentially and reprojects them
onto the sphere; the target-offset example above leaves it at zero.

The fetus mesh stays fixed and the particle cloud's rotation is evaluated at the
same frozen time for every image. The original yellow halo is enabled by default:
the native signed fetus mask is enlarged by 1.1, attenuated, and colored with the
original additive palette. Capture evaluates the stationary feedback separately
for each camera, with no history shared between views. Temporal averaging (motion
blur) and scripted fades stay disabled. Particles retain the original additive flare texture
and are drawn as screen-aligned square sprites: they face every capture camera.
Their pixel size scales with focal length and inverse depth, preserving their
world size at other resolutions/FOVs. Capture-only options
`--gsplat-particle-size-scale S` and `--gsplat-particle-cloud-scale C` multiply
the sprite's side length and the frozen particle centers' spread about native
origin, respectively (both default 1, accepted range 0.1..10). Size 2 doubles
width and height; cloud 2 expands the native cube from roughly +/-5 to +/-10
world units, keeping the same 300 particles. Rendering, visibility, sparse seeds,
`particles.csv` and actual geometry bounds all use the scaled geometry.
Camera planning retains the native enclosure, so expansion does not move cameras
farther away. The widened cloud can extend beyond the frame and surround cameras.
The original environment mapping
remains view-dependent. Capture lifts the native far clipping distance so a larger
orbit does not silently remove geometry.

The apparent surrounding sphere is actually a **directional panorama**, not a
finite mesh. It is rendered in the PNGs, but contributes no fictitious background
points. The halo is a screen-space appearance effect, not duplicated 3D geometry;
it creates no extra seed points or opaque observations. The sparse cloud contains visible fetus surface samples and frozen
particle centers observed in at least two training views. Nonblack sprite pixels
participate in visibility checks; blank corners do not hide the underlying mesh.

Options: `--frames`, `--width`, `--height`, `--gsplat-time`, `--gsplat-fov`,
`--gsplat-radius`, `--gsplat-end-radius`, `--gsplat-halo on|off`,
`--gsplat-sampling orbit|octahedral`, `--gsplat-target-offset`, `--gsplat-lateral-offset`,
`--gsplat-particle-size-scale`, `--gsplat-particle-cloud-scale`,
`--gsplat-validation-every` and `--gsplat-camera-path`. Use a fresh output directory.
Radius `0` chooses automatic framing. Final radius `0` keeps that radius constant;
a positive smaller final radius opts into the previous progressive approach. An
explicit starting radius must enclose the fetus and native particles before capture scaling; an
explicit final radius must enclose the fetus and cannot exceed the starting
radius. Both require a 0.5-unit margin. Close cameras may enter the particle
cloud. Equal starting/final radii give a constant-distance orbit.

CSV replay uses Saari's eight columns (`px,py,pz,tx,ty,tz,hfov_degrees,group`),
with groups `progressive`, `sphere`, `custom`, `octahedral`, `octahedral_target`
or `octahedral_offset`. Variable radii and legacy
origin-centered spheres are accepted, as are exact pole cameras. Cameras must
stay outside the fetus enclosure with the same margin. CSV poses/FOVs replace
the generated path and its radius/FOV/offset options; no offset is applied twice.
Reuse the same resolution and
frozen time, halo setting and particle scales for identical images. Replay uses the CSV's actual
radii, including older progressive paths; it does not force them onto a sphere.
FPS does not advance time. `--gsplat-halo off` provides an A/B comparison at the
same poses without changing geometry or observation tracks.

The output layout and coordinate conversion match Saari and Maku: **import only
`images/` and the three COLMAP text files in `sparse/` together in Postshot**.
There are 270 training views and 30 held-out views in `validation/` by default.
`particles.csv` additionally records all 300 frozen native world centers and their
world-space sprite size. Its `point3D_id` identifies the candidate seed; that ID
can be absent from `points3D.txt` when it has insufficient visible observations.
`capture.json` records the orbit center, actual starting/final/minimum/maximum
radii, geometry bounds, path kind, halo mode, frozen time, counts and completion status.
It also records the sampling mode, octahedral grid size, requested/applied origin
and target offsets, and target distribution. During CSV replay, applied offsets
are zero because the saved poses already contain any perturbation.
Particle scales remain active during CSV replay because they describe the scene,
not the cameras. They are saved in `capture.json`, together with the scale origin
and the native enclosure used for camera framing.

The Feta CTest checks PNG integrity, native/COLMAP projection agreement,
reciprocal tracks, particle coordinates, constant radius, optional progressive
approach, halo A/B consistency, hemisphere coverage, complete mesh framing, exact reversed CSV replay,
legacy CSVs, repeated views, frozen-time changes, pole cameras and input validation.
The additional C++ test compares the capture halo against converged normal playback
at three fixed poses (maximum difference 1/255 per channel from native integer
temporal averaging), and checks additive behavior, untouched mesh pixels and ownership.
The octahedral CTest checks the inverse grid mapping, constant radius, origin
and target offsets, unchanged origins for target offsets, polar cameras, invalid
parameters, and byte-identical images after reversed CSV replay.
The native `feta` renderer's 35-frame comparison, WAV and manifest remained
byte-identical. The new full dataset has 270 training views, 30 reserved views and
6,884 initial points. Reconstruction quality still needs assessment in Postshot,
particularly for the screen-space halo, directional background and additive particles.
See [Feta capture notes](../documentation/forward-feta-gsplat-capture.md).

### Demo sequence exports

```powershell
cpp-offline/build/forward-export --output cpp-offline/output --frames 250
```

macOS / POSIX shell form:

```bash
./cpp-offline/build/forward-export --output cpp-offline/output --frames 250
```

Longer intro validation run:

```powershell
cpp-offline/build/forward-export --output cpp-offline/output-intro --frames 1065 --intro-frames-per-row 1
```

Saari 3D pass run:

```powershell
cpp-offline/build/forward-export --sequence saari --output cpp-offline/output-saari --frames 3136 --intro-frames-per-row 7
```

Kukot 3D pass run:

```powershell
cpp-offline/build/forward-export --sequence kukot --output cpp-offline/output-kukot --until-song-position 0x0D00
```

Maku terrain pass run:

```powershell
cpp-offline/build/forward-export --sequence maku --output cpp-offline/output-maku --until-song-position 0x1000
```

Watercube mixed pass run:

```powershell
cpp-offline/build/forward-export --sequence watercube --output cpp-offline/output-watercube --until-song-position 0x1300
```

Generated output:

- `cpp-offline/output/frames/frame_000000.tga`
- `cpp-offline/output/audio/forward.wav`
- `cpp-offline/output/manifest.csv`
- `cpp-offline/output/log.txt`

Current audio status:

- `intro`, `saari`, `kukot`, `maku`, and `watercube` now write native stereo `16-bit PCM` module audio directly from `mods/kuninga.xm` and `mods/jarnomix.xm`.
- `kukot` now slices `jarnomix.xm` from its real handoff point (`0x0700`) before writing audio or song-position events.
- `maku` now slices `jarnomix.xm` from its real handoff point (`0x0D00`) before writing audio or song-position events.
- `watercube` now slices `jarnomix.xm` from its real handoff point (`0x1000`) before writing audio or song-position events.
- `bootstrap` still falls back to silence because it remains a placeholder scene outside the current preservation path.
- intro/saari/kukot/maku/watercube visual scripting now advances from the native XM song-position timeline derived from audio sample position.

## Mux with FFmpeg

Archive-quality:

```powershell
cpp-offline/scripts/mux_master.bat cpp-offline/output
```

macOS / POSIX shell form:

```bash
sh cpp-offline/scripts/mux_master.sh cpp-offline/output
```

Distribution copy:

```powershell
cpp-offline/scripts/mux_h264.bat cpp-offline/output
```

macOS / POSIX shell form:

```bash
sh cpp-offline/scripts/mux_h264.sh cpp-offline/output
```

Full current convenience wrapper:

```powershell
cpp-offline/scripts/export_intro_full.bat
```

macOS / POSIX shell form:

```bash
sh cpp-offline/scripts/export_intro_full.sh
```

The Windows wrapper uses the PowerShell merge script, and the POSIX wrapper uses `python3`, to combine the per-sequence exports into the final current-full output.

That wrapper:

- configures and builds `forward-export`
- resolves segment lengths from native XM song positions, then exports the complete current intro window through `0x1024` plus a short post-roll, followed by the current `saari` window through `0x0700`, the current `kukot` window through `0x0D00`, the current `maku` window through `0x1000`, the current `watercube` window through `0x1300`, the current `feta` window through `0x1600`, and a standalone `uppol` credits tail
- writes outputs under `cpp-offline/output-full-current`
- muxes `forward_full_current_master.mkv` and `forward_full_current_h264.mp4` when `ffmpeg` is available

## Standalone Release Packaging

From the repository root:

```powershell
package_forward_cpp_offline.bat
```

This builds the `Release` exporter, stages a standalone package under:

```text
cpp-offline/dist/forward-cpp-offline-win64
```

and also writes a zip archive next to it:

```text
cpp-offline/dist/forward-cpp-offline-win64.zip
```

The packaged folder includes:

- `forward-export.exe`
- `render_forward_full.bat`
- `scripts/merge_current_full_outputs.ps1`
- `scripts/mux_master.bat`
- `scripts/mux_h264.bat`
- `original/forward/...` with the original asset tree

The package is self-contained for the current C++ offline workflow: run `render_forward_full.bat` from the package root and it writes the offline export under `output/` by default.

## Immediate Next Porting Steps

1. Finish the source-faithful `mute95` validation against Java captures.
2. Tighten the remaining `saari` camera timing, depth-sort, and raster parity against the Java captures now that the terrain/material path is source-shaped.
3. Keep tightening `kukot` toward Java parity, with the next likely wins being near-plane clipping, later-shot flare occlusion, and any remaining quaternion-spline drift against the reference captures.
4. Tighten `maku` timing against the Java capture checkpoints now that the tiled terrain/camera path is source-shaped.
5. Tighten `watercube` env-mesh lighting, near-plane behavior, and Java face-mode `49` parity against the frozen captures now that the broad composition is source-shaped.
6. Reuse the new direct indexed GIF path for `uppol` and the remaining palette-driven routines.
7. Use the native XM sequencer / sample timeline to drive the remaining scene windows beyond the current `intro` / `saari` / `kukot` / `maku` / `watercube` scope, starting with `feta`.
