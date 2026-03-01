# Forward Saari Finalization Roadmap (Source-Faithful)

## Goal

Finalize `saari` (`maajmka`) to a comprehensive, source-faithful implementation in C++, with no intentional visual approximation.

Primary source:
- `reverse/cfr_single/maajmka.java`

Supporting sources:
- `reverse/cfr_single/kmjakmk.java` (terrain + water/reflection renderer)
- `reverse/cfr_single/kaajkka.java` (ASE scene/track playback)
- `reverse/cfr_single/kajakmk.java` (track interpolation runtime)
- `reverse/cfr_single/forward.java` (timeline/messages)

---

## Current C++ Status (what is already good)

Already present and generally correct:
- Sky/backdrop asset path and spherical backdrop pass (`tai1sp.jpg` + `half8.igu`).
- Heightmap-derived mountain mesh from `saarih15.gif`.
- `meditate` and `klunssi` objects loaded from `asses/alku6.ase`.
- `klunssi` scripted per-frame extra rotation and `meditate` extra 180 degree rotation are partially ported.

Open fidelity issues (confirmed):
- Mountain orientation/depth axis mismatch (appears banked/rotated vs original).
- No sea plane / no mirrored reflection pass.
- Camera path and timing differ from Java.
- Saari script messages are only partially emulated (`suh0` and first `suh`), not full row-driven behavior.

---

## Java Behavior That Must Be Matched Exactly

From `maajmka.MajakkA(...)`:
- Camera setup:
  - `jaKkaMa = 1.4`
  - viewport `512x256`
  - camera distance parameter `jAkkaMa = 250`
- Terrain object is created by `KAmAjAK(...)`:
  - height from `images/scape/saarih15.gif`
  - color from `images/scape/saari.gif`
  - height preprocessing includes `-16` bias
  - scale constants `200.0 / width` and `0.16`
  - uses two 256x256 texture halves from `saari.gif` (top and bottom)
- Scene time driving:
  - `this.kaMAJak.kAMAJaK(f * 1.16f, this.KAmaJak);`
  - this `1.16` multiplier is part of original behavior
- Klunssi override each frame:
  - reset matrix then X/Y/Z rotations with `f/3`, `2f/3`, `3f/3`
- Meditate override:
  - add constant `PI` rotation each frame
- Shock/noise overlay:
  - row-randomized subtractive operation using prebuilt LUT (packed renderer semantics)

From `forward.java` script for saari:
- `show saari` at module 2 row `0x0000`
- messages:
  - `0x0000`: `suh0`
  - `0x0100`: `suh`
  - `0x0600`: `suh`
  - `0x0608`: `suh`
  - `0x0610`: `suh`
  - `0x0618`: `suh`
  - `0x0620`: `suh`
  - `0x0628`: `suh`
  - `0x0630`: `suh`
- handoff: `show kukot` at `0x0700`

From `kmjakmk`:
- Terrain/water is not a single plain mesh pass; it has dedicated dual-pass logic when constructed with `true`.
- Reflection path is generated internally (mirrored geometry path + alternate material mode `259`).
- Surface/depth logic uses original engine axis conventions (height is handled on the renderer's Z axis path).

---

## Why the Current Port Diverges

1. Terrain renderer mismatch:
- C++ currently renders a generic height mesh once, with a manual transform (`rotation -0.04`, fixed translation).
- Java uses `kmjakmk` specialized rendering with mirrored/reflection branch and dual texture usage from `saari.gif`.

2. Axis/orientation mismatch:
- Current C++ heightmap mesh is built as `(x, y=height, z)`.
- Java terrain path stores height into the engine's Z/depth coordinate path in `kmjakmk` internals.
- This is the most likely reason for the apparent 90 degree banking/depth-axis mismatch.

3. Timing/interpolation mismatch:
- C++ samples ASE camera/object tracks linearly at `scene_seconds * 1000`.
- Java feeds tracks with `f * 1.16 * 1000` and `kajakmk` interpolation logic (not simple linear).

4. Message parity mismatch:
- C++ currently auto-triggers only early shock events.
- Java behavior is script-row driven for all saari `suh*` events.

---

## 3-Part Finalization Plan

## Part 1: Port `kmjakmk` Terrain + Water/Reflection Core (no approximation)

Implement a dedicated Saari terrain renderer matching `kmjakmk` behavior:

1. Geometry and axis parity:
- Build terrain vertices with Java axis semantics (height on Z-path semantics, not current generic Y-height assumption).
- Remove ad-hoc terrain transform fudge (`-0.04`, hard-coded translation) once parity renderer is active.

2. Texture split parity:
- Preserve both halves of `saari.gif`:
  - top half (terrain/main)
  - bottom half (water/reflection usage)
- Match `aKkaMaj` / `AkKAMaj` assignment behavior.

3. Reflection pass parity:
- Add the mirrored branch as in `kmjakmk` (`AKKAMaj == true`) and material mode split (`3` + `259` equivalent behavior).
- Use same per-vertex culling/depth gate logic ordering before rasterization.

Acceptance for Part 1:
- Sea is visible.
- Mountain reflection appears and tracks camera as in original.
- Mountain no longer appears 90 degree banked relative to reference.

## Part 2: Exact ASE Playback Timing + Object Transform Parity

Replace simplified track playback with Java-equivalent timing/interpolation:

1. Time domain:
- Drive saari animation with `f * 1.16` before conversion to track sample units.
- Stop using raw `scene_seconds * 1000` as the final timing domain.

2. Interpolation parity:
- Port `kajakmk.KKAmAJA(...)` behavior for position and rotation tracks (same tangent/spline handling and quaternion path).
- Keep axis-angle accumulation order consistent with Java.

3. Object override parity:
- Keep `meditate` and `klunssi` per-frame overrides exactly in Java order:
  - evaluate track transform
  - then apply per-frame scripted override logic

Acceptance for Part 2:
- Camera framing/progression matches original shot timing through full saari span.
- Meditate/klunssi orientation progression no longer drifts from capture.

## Part 3: Script-Exact Message Wiring + Validation Harness

1. Row-accurate messages:
- Trigger saari messages from module/order/row exactly as script defines (all rows listed above), not heuristic seconds.

2. Shock overlay parity:
- Match LUT build, scanline permutation, and subtractive math path to Java packed behavior.
- Verify intensity/decay envelope:
  - `suh` -> `100`, decay `200`
  - `suh0` -> `68`, decay `0`

3. Capture checkpoints:
- Add saari validation captures at:
  - `0x0000`, `0x0100`, `0x0600`, `0x0608`, `0x0630`, `0x0700`
- Store frame + metrics (camera pose, playback time, shock state, active message index) for side-by-side review.

Acceptance for Part 3:
- Saari sequence reacts to script messages exactly at intended rows.
- Transition into kukot at `0x0700` is timing-correct.
- Side-by-side comparison shows no major camera/sea/reflection mismatch.

---

## Implementation Order Recommendation

1. Part 1 first (terrain/water/reflection core) to fix structural visual mismatch.
2. Part 2 second (camera/object playback fidelity) to align motion and shot composition.
3. Part 3 last (message + validation) to lock synchronization and finish pixel-level validation.

