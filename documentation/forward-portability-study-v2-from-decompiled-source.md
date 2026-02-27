# Forward Portability Study (V2, from Decompiled Sources)

## Scope

This study is based on the regenerated source tree:

- `reverse/cfr_single` (canonical merged output)
- `reverse/procyon_single` fallback for `DeviceMSbase`
- `reverse/logs/cfr_single_status.tsv` and `reverse/logs/decompilation-summary.md`

## What We Have Now

- Decompiled Java files: `88`
- Total decompiled LOC: `16189`
- Class graph signals:
  - files with `extends`: `34`
  - files with `implements`: `14`
  - interfaces: `4`
- Scene/effect polymorphism:
  - `mmjjmma` subclasses: `6` (`saari`, `kukot`, `watercube`, `maku`, `feta`, `mute95`)
  - `majjkka` subclasses: `2` (`domina`, `uppol`)

## Decompilation Quality Impact

- Full class coverage exists, but some methods are still structurally messy (`Unable to fully structure code` style output).
- Main hotspots with decompiler artifacts:
  - `mmaamma`
  - `kmajkka`
  - `kmaamka`
  - `kmjjkka`
  - `kmjjmmk`
  - `kaajmma`
  - `muhmu/hifi/device/DeviceSun`
- `DeviceMSbase` was replaced with a clean Procyon output and the CFR-failed version is preserved.

Conclusion: this is enough to port, but not enough to treat as clean maintainable source without targeted manual cleanup.

## Confirmed Architecture (from code)

1. App shell and timeline:
- `forward` + `mmjamma`
- Applet lifecycle, command-line/app params (`nosound`, `1x1`)
- Script command interpreter (`init`, `show`, `msg`, `mod`, `kill`, `shutdown`)
- Event scheduler (`kajjmmk`, “Muhmu Event Pipe”)

2. Scene system:
- Abstract scene bases: `mmjjmma` and `majjkka`
- Per-part scene implementations dispatch through shared timeline/messages

3. Software renderer:
- CPU frame buffers (`mmaamma`, `kmajkka`)
- AWT image bridge via `MemoryImageSource`/`ImageConsumer` (`mmajkka`, `mmjamka`)
- Many custom pixel operations, palette transforms, blend/fade/warp routines

4. Geometry and assets:
- Math/vector/uv primitives (`mmajmma`, `mmjakka`, `kmajkmk`)
- Mesh/face pipeline (`mmajmmk`, `kmaamma`)
- Asset loading/parsing from `.ase` and `.igu` (`kaajkka`, `kajamka`)

5. Audio:
- XM/MOD loader and song data (`majjmka`, `maajmmk`, related classes)
- Mixer stack (`majjmma`, `mmjjkmk`)
- Legacy device backends (`MAD`, `DeviceMS_IE3/IE4`, `DeviceSun`, `DeviceNoSound`)

## C vs C++ Decision

Short answer: yes, C++ is the more straightforward target.

Why C++ is better for this codebase:

1. The code is object-centric, not pure procedural:
- class hierarchies (`mmjjmma`, `majjkka`, `mmajkmk`)
- polymorphic dispatch at runtime (scene/update/render/message entrypoints)

2. Data+behavior are tightly coupled:
- renderer and mesh classes mutate internal state across many methods
- direct Java-to-C struct/function split in C would be mechanical but high-friction

3. Porting effort in C would add manual emulation work:
- pseudo-vtables/function pointer plumbing
- manual ownership discipline everywhere
- more risk of behavioral regressions in timing/render/audio state transitions

4. C++ lets you preserve structure first, optimize/refactor later:
- near-1:1 class translation
- staged modernization (`std::vector`, RAII, stronger types) once behavior matches

When C could make sense:

- If your final goal is a tiny freestanding runtime with very strict C-only constraints.
- That is a second-stage optimization target, not the fastest preservation path.

## Revised Porting Plan

1. Canonicalize source first:
- keep current obfuscated names for correctness
- only rename after baseline parity

2. Create C++ runtime layers:
- `core/` translated engine classes
- `platform/` SDL2 window/input/timer/frame-present
- `audio/` XM playback backend (start with `libxmp`)

3. Port in this order:
- math + buffer classes (`mmajmma`, `mmjakka`, `kmajkmk`, `mmaamma`, `kmajkka`)
- mesh/object pipeline (`mmajmmk`, `kmaamma`)
- timeline + scene dispatch (`forward`, `kajjmmk`, `mmjjmma`/`majjkka` subclasses)
- audio loader/mixer (`majjmka`, `majjmma`, `mmjjkmk`)
- platform glue (replace applet/AWT/audio devices)

4. Handle tricky decompiler blocks by bytecode verification:
- use `javap -c -p` when logic is ambiguous in a method

## Effort Estimate (Updated)

For one experienced engineer:

1. C++ path:
- Playable preservation port (not pixel-perfect): `4-6 weeks`
- High-fidelity preservation (timing/audio/visual parity): `8-12 weeks`

2. Pure C path:
- Playable preservation port: `7-10 weeks`
- High-fidelity preservation: `12-18 weeks`

Delta is mostly from manual OO emulation and higher regression/debug cost in C.

## Recommendation

Use C++ for the main port.

- Keep architecture close to decompiled Java first.
- Use SDL2 for presentation/input/audio device abstraction.
- Keep software rendering on CPU.
- Use `libxmp` first, then decide if original mixer behavior must be replicated.

If a C target is still desired, do it as a second stage after a working C++ reference implementation.
