# forward native harness (macOS ARM first step)

This is the native harness for starting the C++ port on modern systems.

Current focus is the original `1x1` visual path (`512x256` logical buffer at full-quality sampling).

## Build

```bash
cd port
# libxmp is required (Homebrew: brew install libxmp)
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
- `p` : toggle quick-win `phorward` post layer

## Core Port Scaffold

Initial minimal 3D core now exists under `src/core/`:

- `Vec2.h`, `Vec3.h`, `Vertex.h` (basic math + vertex shape)
- `Surface32.h/.cpp` (software 32-bit framebuffer with double buffer semantics)
- `Mesh.h/.cpp` (positions, optional texcoords, triangle indices)
- `MeshLoaderIgu.h/.cpp` (loader for the `3DSRDR` text `.igu` mesh dumps used by forward)
- `Image32.h/.cpp` (minimal image decoder path using stb_image for original JPG/GIF assets)
- `Camera.h`, `Renderer3D.h/.cpp` (software transform/projection + near-plane clipping + backface culling + z-buffer + textured/fill pipeline + wire overlay)
- `Timeline.h/.cpp` (minimal keyframed scene driver feeding object/camera state)

## Notes

- Logical framebuffer is fixed at `512x256`.
- Presentation uses SDL texture upload + nearest filtering.
- Lowres and nosound mode switches are intentionally omitted.
- Runtime now prefers `../original/forward/meshes/fetus.igu` (fallback to `half8.igu` then `octa8.igu`).
- First forward-looking scene pass (`feta`-inspired): `fetus.igu` rendered with `images/babyenv.jpg` texturing and `images/flare1.jpg` additive flare layer.
- Quick-win original asset emergence: post layer now uses `images/phorward.gif` (and `images/back.gif` fallback for secondary blending) with scroll/fade compositing.
- XM playback/timing is now integrated through `libxmp` using the original modules:
  - `mods/kuninga.xm` (mod1)
  - `mods/jarnomix.xm` (mod2)
- Scripted sequence mode (`mute95 -> domina -> saari`) now follows module row cues:
  - mute95 -> domina at `0x0d00` on mod1
  - switch to mod2 at `0x1024` on mod1
  - domina -> saari at `0x0700` on mod2
