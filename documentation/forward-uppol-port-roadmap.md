# Forward Uppol Port Roadmap (Source-Faithful)

## Goal

Port the final `uppol` routine (`mmaakmk`) as a source-faithful C++ scene, then wire it into the global script so it starts after `feta` exactly like the original (`show uppol` at module 2 row `0x1600`).

Scope constraint for this port:
- Do **not** implement interactive URL/mail clicking behavior.
- Keep link lines as visual text only (including alignment/underline/hover tint if desired), without opening URLs.

Primary source:
- `reverse/cfr_single/mmaakmk.java`

Supporting sources:
- `reverse/cfr_single/forward.java` (script order + input/status behavior)
- `reverse/cfr_single/kajakka.java` (`images/phorward.gif` cache entry point)
- `reverse/cfr_single/kmajkka.java` / `reverse/cfr_single/mmajkka.java` (indexed blit + palette/display pipeline)

---

## What The Original `uppol` Actually Does

From `mmaakmk`:
- Scene type is 2D routine (`majjkka`), not 3D (`mmjjmma`).
- Logical resolution is fixed `512x256`.
- Background source is `images/phorward.gif` loaded through `kajakka.kAMAjaK(...)`.
- Builds a new indexed working surface and copies palette with:
  - `R = 0`
  - `G = 0`
  - `B = source.B`
  This creates the blue-toned backdrop.
- Per frame:
  1. Vertical wrapped blit of the source image into working surface using frame counter:
     `scrollY = -(frameCount * 256 % sourceHeight)`.
  2. Push indexed pixels/palette to image producer.
  3. Draw that image to offscreen graphics.
  4. Draw rolling credits text (`Courier`, bold, size 16, line step 26, speed `f*25.0`).
  5. Draw offscreen result to output.
  6. Increment frame counter.
- Text control prefixes in the source table:
  - default: centered
  - `l_`: left aligned
  - `r_`: right aligned
  - `__`: URL/mail line marker in original (for this port: visual-only, no click action).
- Interactive lines in original data:
  - `mailto:komplex@jyu.fi`
  - `http://www.jyu.fi/komplex`
- Original Java has link-click handling and a `50ms` click feedback pause.
- For this port, that interaction path is intentionally omitted.

Timing detail that matters:
- Background scroll is frame-count based.
- Credit crawl position is scene-time based (`f`).
Keeping that split is required for faithful motion.

---

## 3-Part Implementation Roadmap

## Part 1: Indexed Uppol Rendering Core (No ARGB Approximation)

Implement a dedicated indexed path for `uppol`:
- Add an indexed surface utility in `port/src/core/` matching the needed `kmajkka` behaviors:
  - palette arrays (`r[256], g[256], b[256]`)
  - clipped/wrapped blit from source indexed image to destination (`AMaJAkk` equivalent)
  - palette update/rebuild step (`aMaJAkk` equivalent)
  - conversion/presentation to current `Surface32`.
- Load `images/phorward.gif` with:
  - pixel indices
  - global palette
  (not only decoded ARGB).
- Recreate uppol init palette transform exactly:
  - zero red/green, keep source blue channel.
- Keep frame counter behavior exactly for scroll:
  - `scrollY = -(frameCount * 256 % sourceHeight)`.

Acceptance for Part 1:
- Blue background tone and scroll cadence match Java behavior frame-for-frame at same FPS.

## Part 2: Credits Text Parity (Interaction Explicitly Omitted)

Port the text system from `mmaakmk` exactly:
- Keep the original string table and prefix parser (`l_`, `r_`, `__`).
- Use the same layout constants:
  - `lineHeight = 26`
  - `font = Courier Bold 16`
  - center x from `(leftMargin + rightMargin)/2` with right margin `512-150`.
- Render `__` lines as non-interactive credit lines:
  - keep underline and alignment behavior
  - optional static highlight tint is allowed
  - do not open URLs, do not implement click state machine.

Acceptance for Part 2:
- Text crawl speed/positioning and formatting match original, with link lines displayed but non-interactive by design.

## Part 3: Global Script Wiring After `feta` + Validation

Integrate into current C++ script flow (`port/src/main.cpp`):
- Add `SceneMode::kUppol`.
- Add script row constant:
  - `kMod2ToUppolRow = 0x1600`.
- Keep existing switch to `feta` at `0x1300`.
- While running `feta` in script mode, switch to `uppol` when:
  - module 2 row reaches `0x1600` (audio-driven path), or
  - fallback timeline reaches calibrated equivalent (no-audio path).
- Update labels and optional key shortcut for direct `uppol` testing.

Validation pass:
- Compare native vs reference capture at rows:
  - `0x1300` (feta start),
  - `0x1520`, `0x1530` (feta black events),
  - `0x1600` (uppol handoff),
  - then several later rows for credit crawl alignment.
- Verify `__` lines render correctly as text/underline only (no URL opening).

Acceptance for Part 3:
- Scene order is `... -> watercube -> feta -> uppol` with handoff exactly at module 2 row `0x1600`, and `uppol` rendering/interaction matches original behavior.
