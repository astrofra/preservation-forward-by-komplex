# forward native harness (macOS ARM first step)

This is a no-audio native harness for starting the C++ port on modern systems.

## Build

```bash
cd port
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Run

```bash
./build/forward_native --nosound
```

Optional flags:

- `--1x1` : open in 1x scale (512x256 window)
- `--scale N` : set startup window scale (1..16)

## Controls

- `Esc` or `q` : quit
- `Space` : pause timeline
- `f` : toggle fullscreen desktop

## Notes

- Logical buffer is fixed at `512x256` and rendered in software.
- Presentation uses an SDL texture upload with nearest filtering.
- This is the runtime shell to begin porting Java classes and scene logic.
