# Forward: first native-resolution browser implementation

Date: September 24, 2026.

## Scope and source separation

The first implementation is in [`cpp-wasm/`](../cpp-wasm/README.md), with a separate
source tree, CMake project, native SDL2 host, and browser host. **No file under
`cpp-offline/` was modified.** Runtime artwork and music are shared, unchanged,
from `original/forward/`.

The initial copy contains 43 engine/support files from revision `148fb742`, whose
offline engine is unchanged from the feasibility study's `0e60e500` baseline.
The inherited `forward_offline` C++ namespace is retained in copied engine code
to avoid a rename-only diff. New runtime and host code uses `forward_player`.
There is no source or binary dependency on the offline project.

The implemented edition fixes rendering at 512×256 and advances visual state at
50 Hz. CSS/window/fullscreen scaling affects presentation only. The 1024×512 and
2048×1024 renderer changes from the feasibility study are outside this edition.

## Implemented components

| Component | Implementation |
| --- | --- |
| Continuous score and stateful visual stepping | [`demo_runtime.cpp`](../cpp-wasm/src/app/demo_runtime.cpp) |
| Resumable native XM preparation | [`module_player.cpp`](../cpp-wasm/src/audio/module_player.cpp), in the independent copy |
| Native SDL2 playback | [`native_main.cpp`](../cpp-wasm/src/host/native_main.cpp) |
| Shared SDL texture presentation | [`sdl_view.cpp`](../cpp-wasm/src/host/sdl_view.cpp) |
| WASM entry points | [`web_main.cpp`](../cpp-wasm/src/host/web_main.cpp) |
| Web Audio transport and page lifecycle | [`player.js`](../cpp-wasm/web/player.js) |
| Original-inspired page | [`index.html`](../cpp-wasm/web/index.html) and [`player.css`](../cpp-wasm/web/player.css) |
| Build and local server | [`CMakeLists.txt`](../cpp-wasm/CMakeLists.txt), Windows build scripts, [`serve.py`](../cpp-wasm/serve.py) |

The browser synthesizes music from the original XM modules in C++/WASM during
preparation, then copies each small PCM block into a browser-owned float32 stereo
buffer. Visuals render in real time. No pre-rendered movie or downloaded audio
recording replaces the demo.

The audio buffer is scheduled after an explicit Start gesture. The visual host
uses Web Audio output timestamps where available, with an estimated-latency
fallback. Repeated display callbacks do not advance scene state. Bounded catch-up
executes every required visual tick; sustained overload pauses playback.

The native host uses SDL2 queued PCM and a one-period latency correction. This is
an approximate audible cursor and should not be mistaken for a measured hardware
position. The browser host has its own Web Audio transport; SDL handles its video.

## Continuous timeline

Audio uses 22050 source sample frames per second. Device output may use another
rate; the observed browser context used 48000 Hz. Scene handoffs use the source
timeline, independently of output resampling.

| Scene | Absolute source sample at show |
| --- | ---: |
| `mute95` | 0 |
| `domina` | 1,696,968 |
| `saari` | 2,173,248 |
| `kukot` | 3,428,976 |
| `maku` | 4,505,315 |
| `watercube` | 5,043,485 |
| `feta` | 5,581,654 |
| `uppol` | 6,119,824 |

The module change at `kuninga.xm:0x1024` occurs at exactly 98.56 seconds, without
the export wrapper's 12-frame post-roll. Module 2 plays continuously. Feta's
preparation at `0x1230` happens while Watercube remains visible; its audio prefix
is not replayed. Script commands retain sequential row-wait semantics before
their resolved absolute sample timestamps are merged.

Total playback is **6,913,624 samples**, approximately **313.543 seconds**. This
includes the first edition's explicit 36-second credits tail inherited from the
offline wrapper. The ending duration remains a preservation choice, not proof
of an original historical endpoint.

## Repairs discovered during implementation

The first arbitrary-block mixer adapter failed the PCM comparison after roughly
5.34 seconds. The inherited ping-pong sample routine changes its rounding state
at the end of a mix call. Splitting a musical tick across multiple calls therefore
changes subsequent PCM. The adapter now synthesizes complete musical ticks into
a small pending buffer and serves arbitrary output blocks from that buffer.
The original mixer arithmetic is unchanged.

SDL's browser backend can adopt CSS dimensions for a resizable window. When the
canvas was initially hidden, that produced a zero-sized backing canvas. The web
window is now fixed-size at SDL level, while CSS handles display scaling. The
browser check verifies 512×256 backing dimensions before and after a small-screen
resize.

## Validation evidence

Toolchain and host used for the initial checks:

- Windows, Intel Core i7-10700 CPU at 2.90 GHz.
- MSVC 19.41.34120, Visual Studio 2022, CMake 3.30.
- Native SDL2 2.32.10, downloaded from a SHA-256-pinned archive.
- Emscripten 6.0.10; its SDL2 port also uses 2.32.10.
- Chrome 153.0.8010.53, controlled headlessly with Python Playwright 1.63.0.

Observed results:

| Check | Result |
| --- | --- |
| Original offline exporter | Builds successfully; remains unchanged |
| New native and WASM targets | Both compile and link |
| Native SDL integration | One-second playback smoke test passes with SDL dummy video/audio drivers |
| Incremental PCM | Exact match to the batch renderer for nine seconds plus 137 samples of each module, using irregular block sizes from 1 to 16384 samples |
| Row timestamps | Exact match to batch rendering in those audio checks |
| Complete score preparation | No sample gap/overlap; all eight shows and Feta's preparation precede the correct visibility event |
| Stateful progression | Regular ticks and bounded catch-up match; duplicate presentation does not change state; restart reproduces the same image |
| Complete native visual run | All 15,678 ticks render through Uppol; TGA checkpoints and per-tick CSV produced |
| Complete accelerated WASM visual run | All scenes reached through the final tick at sample 6,913,557; no JavaScript exception |
| Complete browser playback at normal speed | Reached `ended` after all 6,913,624 samples, through Uppol, with zero overload pauses and no JavaScript exception |
| WASM versus unchanged exporter | First 3.02 seconds of pre-device PCM are byte-identical; RGB framebuffer at three seconds is pixel-identical |
| Browser controls | Explicit start, pause/resume, mute, restart, fullscreen, and synthetic visibility interruption pass |
| Browser asset deployment | Loads under `/web/`, exercising relative asset paths |

The source checks are in [`runtime_check.cpp`](../cpp-wasm/tests/runtime_check.cpp)
and [`browser_check.py`](../cpp-wasm/tests/browser_check.py). Generated evidence
is under `build/validation/`, which is intentionally excluded from version control.

Native per-scene render timings from the complete deterministic pass follow. They
exclude SDL upload and audio device work, and were collected while browser build
work could also be running. They are indicative measurements of this machine,
not release guarantees:

| Scene | Median tick (ms) | p95 tick (ms) |
| --- | ---: | ---: |
| `mute95` | 0.40 | 0.50 |
| `domina` | 0.11 | 0.13 |
| `saari` | 16.75 | 23.36 |
| `kukot` | 1.56 | 2.52 |
| `maku` | 6.80 | 9.40 |
| `watercube` | 2.08 | 2.61 |
| `feta` | 4.80 | 4.99 |
| `uppol` | 0.12 | 0.14 |

Saari is the first profiling target: its p95 exceeds the 20 ms simulation budget
on this native measurement. The host preserves state when catching up and pauses
on sustained overload; this is preferable to silently changing effect cadence,
but additional optimization may be needed on slower devices.

The complete normal-speed Chrome run used the output timestamp clock and a
48000 Hz audio context. Its largest observed visual callback was approximately
84.3 ms, including catch-up work; no sustained overload pause occurred. This
headless browser run validates transport progression and recovery budget, not
physical sound output or an external A/V latency measurement.

The generated bundle includes a 965,623-byte WASM file and 4,115,378-byte asset
data file, plus the JavaScript loader and page assets, before HTTP compression.
The prepared Web Audio buffer consumes approximately **52.75 MiB**, in addition
to the WASM heap, decoded assets, and GPU resources.

## Remaining limits

This edition preserves the current C++ visual interpretation; it does not repair
every previously documented C++/Java scene difference. The automated checks do
not establish physical speaker-to-display latency or a subjective full-demo
fidelity verdict. The visibility test dispatches the real handler synthetically;
operating-system background throttling and audio device interruptions still need
manual checks. Firefox, Safari, iOS, Android, Bluetooth outputs, and alternate
native operating systems have not yet been validated.

Build and launch instructions are in the [project README](../cpp-wasm/README.md).
