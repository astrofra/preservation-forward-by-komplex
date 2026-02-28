# Forward Watercube Port Roadmap (Source-Faithful)

## Goal

Implement the `watercube` scene (`kmajmka`) as close as possible to original Java behavior and render order, with no intentional design simplification.

Primary source:
- `reverse/cfr_single/kmajmka.java`

Supporting sources:
- `reverse/cfr_single/kaajmma.java` (triangle raster modes, especially `49`)
- `reverse/cfr_single/mmaamma.java` (10-bit packed surface math/blits)
- `reverse/cfr_single/kmjjmmk.java` (flash/noise overlay)
- `reverse/cfr_single/forward.java` (timeline script events)

---

## What The Original Scene Actually Does

From `kmajmka`:
- Initializes a 3D scene from `asses/nosto3.ase` with camera (`jaKkaMa=1.3`, far=`1000`) and Z-up axis (`JakkaMa=(0,0,1)`).  
  Ref: `kmajmka.java:48-55`, `kmajmka.java:49`, `kmajmka.java:51-52`
- Assigns materials per object name:
  - `Box01` -> `images/reunus2.jpg`
  - `TriPatch01` -> dynamic water texture + face mode `49`
  Ref: `kmajmka.java:65-71`
- Adds 1-2 extra IGU meshes (`kluns1`, optional `kluns2`) with env map texture `images/env3.jpg` and animated local rotations each frame.  
  Ref: `kmajmka.java:76-97`, frame update `kmajmka.java:184-191`
- Builds a 256x256 ripple system with two buffers, stamps `images/rinku2.jpg`, combines with `images/riple2.jpg`, and runs a fixed-point wave step using `0x10040100` saturation logic.  
  Ref: `kmajmka.java:106-167`, `kmajmka.java:170-177`
- Per-frame compositing order is strict:
  1. Clear render target
  2. Update 3D objects
  3. Update ripple buffers
  4. Advance ASE scene with `f*1.8 + 2.0`
  5. Render scene
  6. Build/scale texture panel (`images/1.jpg`)
  7. Overlay large scrolling texture (`images/txt1.jpg`) with optional extra strip (`tex0..tex3`)
  8. Flash/noise pass (`kmjjmmk`)
  9. Optional 4-tile shock overlay on `pum`
  Ref: `kmajmka.java:180-233`
- Message triggers alter runtime state:
  - `suh/suh0/suh1/suh2` -> flash envelope
  - `rok` -> camera-roll impulse
  - `pum` -> shock overlay envelope
  - `tex0..tex3` -> strip offset (-80/-160/-240/-320)
  Ref: `kmajmka.java:236-271`

---

## 3-Part Implementation Roadmap

## Part 1: Legacy Pixel Core (must be exact first)

Implement a dedicated legacy 10-bit packed pixel path for this scene (do not reuse only 8-bit ARGB helpers).

Required operations to port from `mmaamma`:
- Saturating add/sub with `0x10040100` carry mask
- `amAjAKK(1)` channel right-shift fade
- Additive blit (`AmAJAKK`)
- Scaled additive blit (`AmAJakK`) with 10-bit fixed stepping
- Horizontal feedback blur (`aMajAKK(float)`)
- Optional temporal blend helpers used by this scene chain

Exact refs:
- `mmaamma.java:81-176`, `324-380`, `473-558`, `602-661`, `663-681`

Acceptance for Part 1:
- Byte/word-level results for these operations match Java output on identical input buffers.

## Part 2: Watercube Scene Logic Port (`kmajmka`)

Port `kmajmka` method-for-method:
- Init (`MajakkA`) with the exact asset set, object-name material setup, and dynamic texture wiring.
- Ripple system init (`KaMAjAK`), wave step (`kAMAjAK`), ring injector (`KAMAjAK`), and ping-pong update (`kaMAjAK`).
- Frame update (`maJakkA`) preserving call order and formulas:
  - `kaMAjAK.kAMAJaK(f*1.8 + 2.0, cam)`
  - `cam.jakkaMa = kAMaJak * 2π`
  - `kAMaJak *= 0.917`

Exact refs:
- `kmajmka.java:44-104`, `106-177`, `180-233`

Raster mode requirement:
- Ensure face mode `49` behaves like Java path in `kaajmma` (textured additive write), not generic shaded triangles.
- Ref: `kaajmma.java:236-259`, `kaajmma.java:337-380`

Acceptance for Part 2:
- Visual layering and per-frame motion match original capture sequence order.

## Part 3: Script Sync + Validation Harness

Wire all `watercube` messages from the global script order rows:
- `show watercube` at `P1 0x1000`, then `pum/rok/suh*`, then `tex0/1/2`.
- Ref: `forward.java:49` and existing extracted timeline doc.

Validation pass:
- Side-by-side recorder (original vs native) at identical order/row checkpoints:
  - `0x1004`, `0x1100`, `0x1200`, `0x1210`, `0x1220`, `0x1230`
- Compare:
  - Camera roll impulse and damping
  - TriPatch water response and texture contribution
  - Overlay strip offsets (`tex0..tex2`)
  - Flash/noise amount envelope

Acceptance for Part 3:
- Checkpoint frames align in composition and timing with only expected randomness variance.

---

## Important Constraint About “Pixel-Perfect”

`watercube` uses many `Math.random()` calls each frame (`kmajmka`, `kmjjmmk`), and original Java does not fix a deterministic seed per run in this scene path.  
This means strict bit-identical frame output run-to-run is not mathematically guaranteed even in the original.

Practical target:
- Keep algorithmic parity (same formulas, same draw order, same random call sites/order).
- For deterministic regression tests, optionally replace randomness with a fixed-seed Java-compatible RNG in a debug mode.
