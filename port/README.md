# forward native harness (macOS ARM first step)

This is the no-audio native harness for starting the C++ port on modern systems.

Current focus is the original `1x1` visual path (`512x256` logical buffer at full-quality sampling).

## Build

```bash
cd port
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Run

```bash
./build/forward_native
```

## Controls

- `Esc` or `q` : quit
- `Space` : pause timeline
- `f` : toggle fullscreen desktop

## Core Port Scaffold

Initial core translation layer now exists under `src/core/`:

- `Vec2.h` (from Java math utility style)
- `Vec3.h` (from `mmajmma` style vector ops)
- `Vertex.h` (from `mmjakka` shape)
- `Surface32.h/.cpp` (minimal 32-bit software surface + double buffering)

## Notes

- Logical framebuffer is fixed at `512x256`.
- Presentation uses SDL texture upload + nearest filtering.
- Lowres and nosound mode switches are intentionally omitted.
- Audio is deferred; next milestone is XM pipeline integration.
