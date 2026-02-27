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

Initial minimal 3D core now exists under `src/core/`:

- `Vec2.h`, `Vec3.h`, `Vertex.h` (basic math + vertex shape)
- `Surface32.h/.cpp` (software 32-bit framebuffer with double buffer semantics)
- `Mesh.h/.cpp` (positions, optional texcoords, triangle indices)
- `MeshLoaderIgu.h/.cpp` (loader for the `3DSRDR` text `.igu` mesh dumps used by forward)
- `Camera.h`, `Renderer3D.h/.cpp` (software transform/projection + filled triangle raster + z-buffer + wire overlay)

## Notes

- Logical framebuffer is fixed at `512x256`.
- Presentation uses SDL texture upload + nearest filtering.
- Lowres and nosound mode switches are intentionally omitted.
- Runtime currently loads `../original/forward/meshes/half8.igu` (fallback to `octa8.igu`) and renders animated filled mesh with z-buffer and wire overlay.
- Audio is still deferred; next milestone is XM pipeline integration.
