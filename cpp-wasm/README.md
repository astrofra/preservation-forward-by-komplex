# Forward — native-resolution real-time player

Independent C++ source tree for the SDL2 and WebAssembly editions of **Forward**
by Komplex. Rendering is fixed at **512×256**, with 50 visual ticks per second.
The original artwork and XM music are loaded from `../original/forward/`.

`cpp-offline/` is unchanged and remains the reference exporter. This project
does not compile or include files from it. See [SOURCE_ORIGIN.md](SOURCE_ORIGIN.md)
for the copied components and revision.

## Run in a browser

The tested toolchain is **Emscripten 6.0.10**, CMake, and Ninja. Activate an emsdk
installation in the terminal first. On Windows, the build script also accepts
`EMSDK` or a repository-local `.local/emsdk/` installation.

From the repository root:

```powershell
cpp-wasm\build_web.bat
python cpp-wasm/serve.py
```

Open **http://127.0.0.1:8080/**. Select a display size of **512×256 (1×)**,
**1024×512 (2×)**, or **2048×1024 (4×)**. Wait for preparation, then click **Start demo**.
Playback shows only the demo, centered on a very dark gray background with a
one-pixel gray border. The selected size fits smaller windows when necessary. Click the demo or
press Space or Escape to pause and reveal the introduction and controls again.
Use Pause/Resume, Restart, Mute, and Fullscreen. Leaving the tab pauses playback;
returning requires Resume. The canvas can fit a small screen or fullscreen
without changing its 512×256 backing resolution.

These are presentation sizes, not additional internal rendering resolutions.
The larger presets use nearest-neighbour enlargement of the original 512×256
pixels, with no smoothing. This is a preservation decision: retain the work's
original raster, geometry, textures, effects, and audio synchronization rather
than reinterpret its appearance through higher-resolution rendering. The size
can also be changed while paused, without restarting or changing the timeline.

On Linux/macOS, after activating emsdk:

```sh
emcmake cmake -S cpp-wasm -B build/wasm -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/wasm --parallel
python3 cpp-wasm/serve.py
```

These commands are portable build instructions; this initial implementation was
validated on Windows. The static website is generated in `build/wasm/web/`.
Deploy **the entire directory**, including `forward.js`, `forward.wasm`,
`forward.data`, HTML/CSS/JavaScript, and `art/`. Serve WASM with the
`application/wasm` MIME type. Localhost or HTTPS is the supported launch path.
Shared memory and COOP/COEP headers are not required by this edition.

## Prepare a website distribution

On Windows, run the following from the repository root:

```bat
package_forward_wasm.bat
```

This rebuilds the Release browser target, checks all required assets, and produces:

- `cpp-wasm/dist/forward-web/`: the complete static website, ready to upload.
- `cpp-wasm/dist/forward-web.zip`: the same files, with `index.html` at the ZIP root.

The script requires the browser build prerequisites above and Python 3 on `PATH`.
It works from any current directory when invoked by its full path. Each successful
run replaces these generated outputs; keep custom website edits in `cpp-wasm/web/`.
The distribution contains only the eight runtime files, including the two images
under `art/`. Original demo assets are already packed into `forward.data`.

Upload **the contents** of `forward-web/` to your website directory, or upload and
extract the ZIP there. For example, uploading to `/demos/forward/` makes the demo
available at `https://your-site.example/demos/forward/`. Keep all files together
and preserve the `art/` subdirectory. No build tools, Python, or server-side code
are needed on the host. Use HTTPS and serve `.wasm` as `application/wasm`; no
special isolation headers are needed. Replace all release files together and
invalidate any hosting/CDN cache when updating an existing deployment.

To preview exactly what will be uploaded:

```powershell
python cpp-wasm/serve.py --directory cpp-wasm/dist/forward-web
```

Then open **http://127.0.0.1:8080/**; opening the HTML directly with `file://`
does not provide the HTTP asset loading required by the browser build.

## Run the native SDL2 player

From the repository root on Windows with Visual Studio C++ build tools:

```powershell
cpp-wasm\build_native.bat
build\player\Release\forward-player.exe
```

CMake uses an installed SDL2 package when available; otherwise it downloads the
SHA-256-pinned SDL **2.32.10** source archive and builds it locally. The first
configuration therefore needs network access. To use an existing source copy
without downloading, set `FETCHCONTENT_SOURCE_DIR_SDL2` when configuring.

Equivalent native CMake commands:

```sh
cmake -S cpp-wasm -B build/player -DCMAKE_BUILD_TYPE=Release
cmake --build build/player --config Release --parallel
```

Run from the **repository root**, so the original asset paths resolve. On
single-configuration generators the executable is `build/player/forward-player`.
Space pauses/resumes, R restarts, F toggles fullscreen, and Escape exits. Losing
window focus pauses playback. No high-resolution render option is exposed.

## Runtime behavior

- The software scenes and XM mixer are compiled into `forward-core`.
- Music is prepared before playback in bounded blocks. The mixer retains complete
  musical tick boundaries internally: the historical ping-pong sample code changes
  rounding state at mix-call boundaries, so splitting those calls arbitrarily
  would change the PCM. The block-size regression check covers this contract.
- `kuninga.xm` ends exactly at row `0x1024`; `jarnomix.xm` then plays continuously
  through all remaining scenes. Audio is never restarted at scene boundaries.
- `feta` receives its preparation message at `0x1230` while `watercube` is visible,
  then appears at `0x1300`. The overlapping offline export slices are not replayed.
- The current credits tail lasts 36 seconds, matching the offline wrapper's
  explicit tail setting. Total playback is 6,913,624 source sample frames,
  approximately 313.543 seconds. This is a defined first-edition endpoint, not a
  newly established historical duration.
- Every 441 source samples advance one visual tick. Repeated presentations do not
  update noise, feedback, or ripple state. Catch-up renders all required ticks.
- Browser PCM is copied directly from each C++ block into a stereo float32
  `AudioBuffer`, approximately 52.75 MiB for this score. The C++ side keeps only a
  small PCM block. Original decoded assets and render surfaces consume additional
  memory.
- Web Audio schedules playback and supplies the output clock. The browser uses
  `getOutputTimestamp()` when valid and a latency estimate otherwise. Muting only
  changes gain. The host pauses on sustained visual overload instead of silently
  skipping simulation steps.
- Native SDL2 queue accounting compensates one backend period, but remains an
  estimate of audible position. It does not expose a hardware playback cursor.

## Validation

Build the native checks and run:

```sh
ctest --test-dir build/player -C Release --output-on-failure
```

This checks batch-versus-block PCM and row timestamps, continuous sample spans,
scene order, Feta preparation, stateful catch-up, repeated presentation, and reset.
The explicit checks are active in Release builds.

For a complete deterministic native visual pass, first create an output directory:

```powershell
New-Item -ItemType Directory -Force build/validation/native
build/player/Release/forward-check.exe build/validation/native
```

It writes a per-tick `trace.csv` and TGA checkpoints. This pass is offline validation
of the real-time runtime; it does not depend on SDL or an audio device.

The browser test needs Python Playwright and an installed Chromium browser:

```powershell
python -m venv .local/browser-test
.local/browser-test/Scripts/python.exe -m pip install playwright
.local/browser-test/Scripts/python.exe cpp-wasm/tests/browser_check.py --browser "C:/Program Files/Google/Chrome/Application/chrome.exe" --full
```

`--full` replays all visual ticks at accelerated speed. `--play-through` instead
adds a full run at normal audio speed. `--reference-intro PATH` compares the
first 3.02 seconds of PCM and the framebuffer at three seconds against an existing
`cpp-offline` intro export containing at least 151 frames. The test starts its own
localhost server and writes results/screenshots to `build/validation/browser/`.
The visibility check exercises the actual handler with a synthetic visibility
event; it is not an operating-system background-throttling test.

See [the implementation report](../documentation/forward-wasm-native-resolution-implementation.md)
for observed results and remaining validation limits. Safari, Firefox, mobile
devices, physical output latency, and subjective full-demo audiovisual fidelity
require separate validation. Existing C++ scene differences from the Java original
are inherited by this edition.
