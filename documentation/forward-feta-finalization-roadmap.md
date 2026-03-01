# Forward Feta Finalization Roadmap (Source-Faithful)

## Goal

Finalize `feta` (`kmaamka`) to a comprehensive, source-faithful implementation based on what is already ported, with no intentional visual approximation.

Primary source:
- `reverse/cfr_single/kmaamka.java`

Supporting sources:
- `reverse/cfr_single/mmaamka.java` (particle cloud object used by feta)
- `reverse/cfr_single/kaaakma.java` (environment/background object)
- `reverse/cfr_single/kmaakma.java` (scene container + render ordering)
- `reverse/cfr_single/mmaamma.java` (`AMAJakK(int,int)` darkening behavior)
- `reverse/cfr_single/forward.java` (script events and message timing)

---

## Current C++ Status (already done)

What is already in place:
- `fetus.igu` mesh rendering with `babyenv.jpg`.
- `kaaakma`-like textured background pass (`images/verax/kosmusp.jpg`).
- `mmaamka`-like particle cloud visuals (`images/flare1.jpg`, ~300 sprites).
- Multi-pass mesh halo approximation around the fetus.
- Script timeline wiring: `watercube -> feta -> uppol` with `feta` show at row `0x1300`.

Main fidelity gaps versus Java:
- No exact port of `kmaamka.KamAJAk(...)` indexed/packed post-composite path.
- No exact handling of `msg feta 1/2`, `blackfeta`, `blackmuna`.
- `mmaamma.AMAJakK(int,int)`-style signed-pixel darkening split is not reproduced for feta.
- Feta is still rendered with a modernized approximation path (`DrawFetaFrame`) rather than full Java-equivalent pipeline state.

---

## Java Behavior That Must Be Matched

From `kmaamka.java`:
- Init:
  - Camera viewport `512x256`, `jaKkaMa=1.9`, position/orientation updates.
  - Scene graph includes:
    - `kaaakma(..., "images/verax/kosmusp.jpg", true)` (octa background path).
    - `fetus.igu` object with `babyenv.jpg` and alpha-bit tagging (`| Integer.MIN_VALUE`) on texture pixels.
    - `mmaamka(300, 20.0f)` in mode `0`, textured by `flare1.jpg`.
  - Calls `kAMAJAk()` palette setup where entry `255` is forced black.
- Per frame (`maJakkA`):
  1. Clear frame.
  2. Animate camera and particle object transforms.
  3. Render scene container (`kmaakma.MajAKkA + MAJAKkA`).
  4. Run `kamAJAk(...)` which uses `KamAJAk(...)` indexed sampling and additive packed blend.
  5. Present frame.
- Messages (`MAJakkA`):
  - `"1"` -> `kAMAJAk()` palette variant (index 255 black).
  - `"2"` -> `KAMAJAk()` palette variant (normal gradient).
  - `"blackfeta"` and `"blackmuna"` store timestamps that drive two-phase darkening in `kamAJAk(...)` through `mmaamma.AMAJakK(n, n2)`.

From `forward.java` script:
- `msg feta 1` at module 2 row `0x1230` (before `show feta`).
- `show feta` at module 2 row `0x1300`.
- `msg feta blackfeta` at `0x1520`.
- `msg feta blackmuna` at `0x1530`.
- then `show uppol` at `0x1600`.

---

## 2-Part Finalization Plan

## Part 1: Exact Feta Render Core Port (replace approximation path)

Port `kmaamka` internals method-for-method into native equivalents:

1. Port `kamAJAk(...)` + `KamAJAk(...)` exactly:
- Keep the fixed scale `1/1.1`.
- Keep 8-bit indexed sampling path and destination write behavior.
- Keep the conditional masked path (`var3`) and signed-pixel logic.
- Keep packed saturating add math (`0x10040100`) semantics.

2. Port message-driven palette and darkening behavior:
- Implement both palette variants:
  - `kAMAJAk()` (`index 255 = black`)
  - `KAMAJAk()` (normal gradient)
- Implement `blackfeta` / `blackmuna` timers and exact mapping to
  `mmaamma.AMAJakK(n*65793, n2*65793)`.

3. Align camera/object motion constants to Java:
- Camera transform expressions in `maJakkA`.
- Particle cloud object transform (`AMaJAKk(-f/2)` style rotation).
- Keep feta-specific values from init (`jaKkaMa=1.9`, object scale/flags).

Acceptance for Part 1:
- Visual behavior and fade progression during `0x1520..0x1600` match Java capture and decompiled formulas, not just close-looking approximation.

## Part 2: Script-Exact Integration + Validation Harness

Integrate full feta messaging and verify at script checkpoints:

1. Script message parity:
- Ensure `msg feta 1` is applied even if sent before `show feta` (as in original order).
- Ensure `blackfeta` and `blackmuna` events are consumed at correct rows.

2. Sequence linkage in full demo:
- Keep global order:
  - `... -> watercube -> feta -> uppol`
  - rows `0x1300 -> 0x1600` for feta span.
- (Clarification) feta is **between** watercube and uppol in the original script.

3. Validation checkpoints:
- Capture/compare at least:
  - `0x1300` (feta start),
  - `0x1520` (`blackfeta`),
  - `0x1530` (`blackmuna`),
  - `0x1600` (handoff to uppol).
- Compare: palette mode, darkening envelope split, halo/composite behavior, and transition timing.

Acceptance for Part 2:
- Feta timing and message responses are script-accurate and scene handoff to uppol occurs exactly at `0x1600`.

