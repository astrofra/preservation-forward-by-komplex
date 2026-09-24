# Forward: C++ real-time player and WebAssembly feasibility study

Assessment date: September 24, 2026. Source revision: `0e60e500`.

## 1. Recommendation and scope

**The browser port is feasible.** The preferred route is to extract a reusable runtime from `cpp-offline`, validate it in a native SDL2 player, and compile the same rendering and sequencing code with Emscripten. Retain the offline exporter as a regression oracle and preservation deliverable.

Use SDL2 for the native window, input, texture presentation, and audio output. Reuse its texture presentation in the browser, with a small Web Audio adapter providing playback and an audio clock. Start with PCM prepared by the existing C++ XM engine before playback; introduce streaming through an AudioWorklet only if measured loading time or memory requires it. Visuals remain rendered in real time in C++/WASM.

The proposed public modes are:

| Mode | Actual render dimensions | Assessment |
| --- | --- | --- |
| Default | 512 × 256 | Best first release target; matches the current C++ renderer |
| High | 1024 × 512 | Feasible after a separate resolution refactor and performance validation |
| Experimental | 2048 × 1024 | Technically plausible; enable only on configurations that pass the full demo benchmark |

All modes retain square pixels and the original **2:1 image aspect ratio**. Here, “1:1” means the original high-quality sampling mode, without its low-resolution 2×2 pixel reduction. It does not mean a square viewport. The original [`forward_1x1.html`](../original/forward/forward_1x1.html) uses a 512×256 applet with `1x1=1`; [`ForwardDemoApp.java`](../java-desktop/src/main/java/ForwardDemoApp.java), in `kaMAjak()`, selects presentation factors of either 1×1 or 2×2.

This is a source-based feasibility assessment, with browser API documentation checked against primary sources. No native player, WASM executable, browser performance measurement, or end-to-end synchronization measurement was produced for this study. `emcc` was not available on the inspected shell's PATH. Performance ratings and effort ranges below are engineering estimates, not benchmark results.

## 2. What already exists

| Component | Observed implementation | Consequence for the port |
| --- | --- | --- |
| Build | [`CMakeLists.txt`](../cpp-offline/CMakeLists.txt): one C++11 executable, `forward-export`; vendored `stb_image` | A small dependency surface, suitable for an Emscripten build |
| Application | [`ForwardApp::run()`](../cpp-offline/src/app/forward_app.cpp): initializes one sequence, prepares its audio, renders a blocking frame loop, writes TGA/WAV/CSV | Requires a callable runtime step and separate output hosts |
| Framebuffer | [`RgbSurface`](../cpp-offline/src/render/rgb_surface.h): a contiguous vector of 32-bit pixels; [`TGA writer`](../cpp-offline/src/render/tga_writer.cpp) extracts ordinary RGB bytes | The final image can become a streaming texture without rewriting the rasterizers |
| Time | [`OfflineTimeline`](../cpp-offline/src/core/offline_timeline.cpp): default 50 fps and 22050 Hz, with an integer number of samples per frame | Preserve the existing 441-sample visual cadence |
| Audio | [`module_player.cpp`](../cpp-offline/src/audio/module_player.cpp): native XM loading, mixing, PCM generation, and sample-indexed song-position events | Reuse the existing music engine and event trace |
| Audio API | [`SequenceAudioRender`](../cpp-offline/src/audio/module_player.h) owns complete PCM and event vectors | It is currently a batch renderer, not a live audio callback API |
| Full playback | [`export_intro_full.sh`](../cpp-offline/scripts/export_intro_full.sh) and [`merge_current_full_outputs.py`](../cpp-offline/scripts/merge_current_full_outputs.py) export and concatenate separate sequences | A continuous full-demo controller does not yet exist |
| Resolution | `ForwardApp::initialize_sequence()` explicitly rejects dimensions other than 512×256 for every non-placeholder sequence | The CLI width/height options do not establish high-resolution support |
| Assets | JPEG/GIF, ASE/IGU, XM loaded directly from `original/forward` | Keep the native loaders and reproduce their paths in a virtual filesystem |
| Reference material | [`reference-capture/`](reference-capture/), the Java reconstruction, scene investigation documents | Existing evidence supports regression checks, but does not prove browser fidelity |

The current source includes `feta` and `uppol`, although parts of [`cpp-offline/README.md`](../cpp-offline/README.md) still describe earlier milestones. Likewise, the historical [macOS audit](forward-cpp-offline-macos-compilation-audit.md) identified `sscanf_s`; the inspected source now uses `std::sscanf`. Neither older limitation should be reported as a new compile blocker.

The four asset directories (`images`, `asses`, `meshes`, `mods`) contain 45 files totalling 4,167,602 bytes. Excluding the historical `meshes/3DSRDR.exe` leaves 44 files and 4,115,378 bytes, approximately 3.93 MiB before compression. This is a practical upper-bound starting set for runtime asset staging, not the final download size: WASM/JavaScript and decoded runtime memory are additional.

Local preservation mapping remains: original release in `original/`, reconstruction evidence in `reverse/`, Java reference in `java-desktop/`, native source in `cpp-offline/`, and validation evidence in `documentation/`.

## 3. First transform the offline application into a reusable runtime

### 3.1. Proposed separation

Keep the code in its current source tree initially and introduce three build targets around one common library:

```text
Original assets + XM modules
              |
         forward-core
    assets / scenes / XM engine
    score / sample timeline / framebuffer
       /            |             \
forward-export  forward-player  forward-web
TGA/WAV/CSV      SDL2 native    SDL2 canvas + Web Audio
```

`forward-core`, `forward-player`, and `forward-web` are proposed names, not existing targets. SDL and browser headers belong in host adapters. The core must remain usable by the dependency-light exporter.

| Responsibility | Proposed interface or owner | Required change |
| --- | --- | --- |
| Asset preparation | `DemoRuntime::initialize(options)` | Separate loading from output-directory creation and playback start |
| Scene and script state | `DemoRuntime` plus a complete score controller | Extract the state currently held by `ForwardApp` |
| Visual progression | `advance_to_sample(source_sample)` | Execute all required fixed visual ticks and script events up to a playback position |
| Frame access | `framebuffer()` | Return the latest image without advancing the simulation |
| Audio preparation | `ScoreAudio` | Produce continuous PCM and absolute event timestamps for both module phases |
| Playback position | Host `AudioTransport` | Expose source-sample position, running/paused state, and clock confidence |
| Presentation | Export writer or SDL presenter | Write a frame or upload it to a texture |
| Diagnostics | Optional trace sink | Record sample, scene, script command, tick, frame cost, and pause events |

Start by moving behavior, without changing scene math, random seeds, mixer gain, or effect cadence. `render()` currently mutates scene state. An initial runtime tick can therefore call the existing render method exactly once per 20 ms step, even when that image is not displayed. A later split into `update()` and `draw()` is useful only where feedback behavior can be preserved and verified.

### 3.2. Native SDL2 milestone

Create a window and streaming texture, load the assets, prepare PCM, then begin audio and visual playback together. Keep close/pause/restart handling responsive. Use either an SDL audio callback reading prepared PCM or bounded `SDL_QueueAudio()` buffering; do not mix the two models on one device.

The callback must only copy or convert prepared audio, update counters, and fill silence when stopped. It must not load assets, execute scripts, allocate large buffers, or render images. Negotiate the device format and resample from the canonical source rate when necessary; never play 22050 Hz samples as though they were 48000 Hz samples.

Native SDL audio accounting is an estimate of audible position. Queue length measures bytes not yet handed to the backend, and cannot locate samples already in hardware buffers. A callback counter similarly measures supplied audio. Track backend buffering and a measured latency correction instead of naming either counter “samples heard.” [SDL queue-size contract](https://wiki.libsdl.org/SDL2/SDL_GetQueuedAudioSize).

The native milestone is complete when the whole score plays in one process, its sample/event trace matches the chosen baseline, and pause/restart and texture presentation work. A window showing one exported scene is only an intermediate checkpoint.

## 4. Reconstruct continuous playback before claiming preserved synchronization

The [global script timeline](forward-global-script-timeline.md) provides the structure, with the repaired Java and raw script as evidence for command semantics:

| Module phase | Position | Action |
| --- | --- | --- |
| `kuninga.xm` | `0x0000` | Show `mute95` |
| `kuninga.xm` | `0x0D00` | Show `domina` |
| `kuninga.xm` | `0x1024` | Switch modules; process the associated clear/kill commands |
| `jarnomix.xm` | `0x0000` | Begin `saari` |
| `jarnomix.xm` | `0x0700` | Show `kukot` |
| `jarnomix.xm` | `0x0D00` | Show `maku` |
| `jarnomix.xm` | `0x1000` | Show `watercube` |
| `jarnomix.xm` | `0x1230` | Deliver the preparatory message to `feta` while `watercube` remains visible |
| `jarnomix.xm` | `0x1300` | Show `feta` |
| `jarnomix.xm` | `0x1600` | Show `uppol`; continue the intended credits audio |

There are three concrete traps in reusing the export wrapper as a player:

1. **The intro has export post-roll.** The default wrapper adds 12 frames after `0x1024`. This must not automatically become a 240 ms delay before starting the second module.
2. **The `feta` slice overlaps the preceding music.** Its audio mapping begins at `0x1230`, although `watercube` exports through `0x1300`. Concatenating those slices repeats the overlapping musical interval. Preserve the preparation event at `0x1230` inside one ongoing module playback.
3. **Segment lengths are rounded to visual frames.** `resolve_sequence_frame_count_for_song_position()` rounds up to a multiple of 441 samples. Repeated concatenation can introduce small overlaps at boundaries. Audio joins must use exact sample positions, independently of frame quantization.

Render `jarnomix.xm` continuously through the second phase rather than restarting or re-slicing it at each scene. Stop module 1 and begin module 2 at a documented source-sample boundary. Keep the current gain choices (`88` for the intro and `128` for module 2) during parity work. Do not add crossfades or normalization without a separate fidelity decision.

Define each event with an absolute source-sample timestamp, module phase, song position, and stable command order. The song position is an order/row identifier, not elapsed time. Phase resets and backward-looking script markers mean that sorting all commands numerically by hexadecimal position changes behavior. Preserve sequential wait/dispatch semantics, including multiple commands reached by the same row notification.

The complete controller also needs explicit handling of lifecycle commands such as `mod`, `killmod`, `show`, and shutdown. The current per-sequence `execute_script_command()` is not a complete full-demo interpreter. Audit `go`, `filmbox`, and `reality` against the Java host to decide which affect presentation and which have already been replaced by the C++ fixed cadence.

The wrapper's 1800-frame standalone credits tail is a 36-second export choice. Define the browser's end condition explicitly after checking the credits and module behavior; do not infer the original duration from that wrapper setting.

Keep two validation references: current per-scene C++ output for renderer stability, and the reconstructed continuous score for musical joins. An intentionally corrected join should not be forced to match an overlapping concatenated movie.

## 5. Timing model

### 5.1. Audio is the master clock

Use one canonical source timeline at **22050 stereo sample frames per second** and a **50 Hz visual simulation**. A sample frame contains one sample for each channel; it is not an individual interleaved `int16_t` value.

```text
source sample rate R = 22050
visual step          = 441 source sample frames
visual target tick   = floor(audible_source_sample / 441)
scene local time     = (tick_sample - scene_start_sample) / R
```

Dispatch script commands using their original event timestamp, then produce the appropriate visual tick. A command between ticks becomes visible on the first following tick. This retains the exporter's visual quantization without shifting the musical event itself.

Do not drive the score from rendered frame count, wall-clock elapsed time, callback invocation count, or `SongPositionTransport`'s legacy frames-per-row approximation. Monitor refresh and browser callback frequency only determine when presentation is attempted.

### 5.2. Browser playback position

For prepared PCM, schedule the audio source at a known `AudioContext` time `t0`. Store the corresponding source offset `S0`. Web Audio supports scheduled source starts and offsets. [Source start semantics](https://developer.mozilla.org/en-US/docs/Web/API/AudioBufferSourceNode/start).

Prefer the output timestamp pair to estimate the audio position at the intended display time:

```text
t_output = stamp.contextTime
           + (display_performance_ms - stamp.performanceTime) / 1000
S        = S0 + floor((t_output - t0) * 22050)
```

This formula is a proposed estimator, derived from the API's mapping between output-stream time and `performance.now()` time. Clamp it to the playable interval; do not extrapolate stale timestamps across pauses or device interruptions. `display_performance_ms` starts as the current performance time; a measured presentation offset may refine it. The API does not report when a pixel physically becomes visible. [Output timestamp definition](https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/getOutputTimestamp).

If the timestamp is unavailable or invalid, use `AudioContext.currentTime` with a calibrated latency estimate and mark that clock as approximate. Feature-detect latency reporting; `outputLatency` is itself an estimate. Do not subtract latency again when an output timestamp has already accounted for it. [Output latency](https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/outputLatency).

Hardware may run at 44100 or 48000 Hz. Keep source-sample positions and device-frame positions distinct; maintain fractional resampling state for streaming. The source timeline need not change when the audio device rate changes.

### 5.3. Preserve feedback effects at every refresh rate

On a 60, 120, or 144 Hz display, present the most recent 50 Hz state as often as useful. Do not call scene `render()` once per display refresh: noise, feedback, averaging, and ripples can change with each call.

Conceptual progression, with `next_tick` initially zero:

```text
target_tick = floor(audio_transport.source_sample() / 441)
while next_tick <= target_tick and within_callback_work_budget:
    dispatch_events_through(next_tick * 441)
    render_one_stateful_tick(next_tick, delta_seconds = 0.02)
    next_tick += 1
present_latest_frame_if_changed()
```

This can discard intermediate presentations, but preserves required simulation and feedback steps. Jumping directly to the newest time or replacing several feedback steps with one large delta is not equivalent. `watercube` already has some tick catch-up logic; other scenes still require their normal sequence of calls.

If sustained overload exceeds the catch-up budget, pause audio and visuals together, rebuild visual state to the agreed pause position if necessary, and offer restart at a lower resolution. Do not slow the score while music continues or silently drop state updates. Changing resolution during playback should remain unsupported initially because history surfaces would need migration.

### 5.4. Start, pause, and visibility

Preload and prepare before enabling **Start**. In the user's click/tap handler, create or resume the audio context, verify it is running, and schedule the common start anchor. Autoplay policy makes that gesture part of the playback design. [Web Audio interaction guidance](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API/Best_practices).

Use an explicit state machine: `loading → ready → playing ↔ paused → ended`, with an error state. Muting should change gain while retaining the audio clock. Restart resets the score, seeds, histories, event cursor, and audio anchor.

When the page becomes hidden, suspend the audio context and visual progression together. Resolve the actual suspension position before committing the pause state. On return, resume from a consistent sample/state pair, requiring a Resume gesture if the browser needs one. Account for system audio interruptions too. Browsers can suspend animation callbacks and throttle timers in hidden pages. [Page Visibility behavior](https://developer.mozilla.org/en-US/docs/Web/API/Page_Visibility_API).

## 6. Audio backend choices

| Option | Benefits | Limitations | Recommendation |
| --- | --- | --- | --- |
| SDL2 audio in both native and WASM builds | Small host API surface; useful first sound test | SDL alone does not expose an exact audible cursor; backend scheduling and latency need verification | Prototype or measured fallback |
| C++-prepared PCM played through Web Audio | Reuses the mixer; explicit scheduled start and output clock; no application audio work during visual rendering | Startup work and whole-score PCM memory | Preferred first browser release, subject to memory/loading gates |
| Incremental mixer or PCM ring feeding AudioWorklet | Bounded audio memory and work away from the visual callback | More state, resampling, underrun handling, and deployment complexity | Follow-up if preparation is too expensive |

SDL2 has a documented Emscripten backend, including audio and WebGL-backed 2D rendering. This makes it suitable for the viewport. It does not require the final browser build to use SDL for audio too. Avoid running two competing playback contexts. [SDL2 Emscripten support](https://wiki.libsdl.org/SDL2/README-emscripten).

For the preferred path, synthesize the actual XM modules with the C++ engine at startup. Convert interleaved signed 16-bit samples into two float channels, preserving gain, and create an `AudioBuffer` at the source rate. Copy into browser-owned storage, then release unnecessary C++ PCM copies. `AudioBuffer` uses separate float32 channel arrays; the buffer's rate is explicit. [AudioBuffer format](https://developer.mozilla.org/en-US/docs/Web/API/AudioBuffer).

Schedule the full continuous score once. An `AudioBufferSourceNode` can only be started once, so restart or offset-based recovery creates a new source node while reusing its buffer. Context suspension can handle ordinary pause/resume for this dedicated player. [AudioBufferSourceNode lifecycle](https://developer.mozilla.org/en-US/docs/Web/API/AudioBufferSourceNode).

### Memory and preparation cost

At 22050 Hz stereo:

| Storage | Bytes per second | Example for 300 seconds |
| --- | ---: | ---: |
| Signed 16-bit PCM | 88,200 | 25.23 MiB |
| Float32 Web Audio channels | 176,400 | 50.47 MiB |
| Both copies alive | 264,600 | 75.70 MiB |

The 300-second duration is an illustration, not a measured demo length. Add mixer scratch, decoded assets, framebuffer histories, WASM heap capacity, and browser/GPU allocations. The current mixer creates an additional packed-frame vector, and sliced sequence rendering can retain a discarded musical prefix temporarily. Repeatedly rendering full prefixes for each scene is unsuitable as the final preparation strategy.

Extract a resumable mixer with persistent song, tick-boundary, voice, and fractional-sample state. It can prepare the whole score in bounded chunks and yield loading progress without requiring streaming during playback. Preserve the current tick rounding across chunk boundaries. Validate block-size independence against the existing batch output before using it for either preparation or live synthesis.

If whole-score memory fails the target-device budget, use a worklet. Emscripten's integrated Wasm AudioWorklet API uses Wasm Workers and supports C/C++ processing callbacks; its documented build route uses `-sAUDIO_WORKLET -sWASM_WORKERS`. Callback code must be bounded and non-blocking, and must use the provided block length rather than assuming it is permanently 128 frames. [Wasm AudioWorklets](https://emscripten.org/docs/api_reference/wasm_audio_worklets.html).

That integrated shared-memory route requires suitable cross-origin isolation deployment. A separately instantiated worklet using message transfer is another architecture and does not inherit the same shared-memory requirement, but needs its own buffering design. Worklets also require a secure context. Do not make shared memory a requirement of the prepared-PCM release. [Wasm Workers deployment requirements](https://emscripten.org/docs/api_reference/wasm_workers.html), [AudioWorklet interface](https://developer.mozilla.org/en-US/docs/Web/API/AudioWorklet).

For streaming, distinguish generated, queued, consumed, and audible samples. A ring-buffer underrun must not advance the demo's source timeline over missing content. Re-anchor after recovery, with visuals paused or reconstructed to match. Never run scene callbacks in the audio thread.

## 7. Viewport and higher render resolutions

### 7.1. Present the existing pixels first

Use an SDL streaming texture and `SDL_RenderCopy()`/`SDL_RenderPresent()`. The software renderer remains responsible for every scene pixel; the GPU initially only presents the resulting rectangle.

The final surface contains `0x00RRGGBB` values, while some internal assets and effects use the historical `R<<20 | G<<10 | B` representation. Convert only at the established final boundary. Choose an SDL pixel format matching the numeric layout, or explicitly repack to opaque RGBA bytes. Verify red/blue order, alpha, row pitch, and orientation with a small color pattern. Treating the high zero byte as an alpha channel can make the image transparent.

Reuse the texture and staging storage. Avoid allocating images or passing individual pixels through JavaScript. Upload only when a new simulation frame exists. Keep nearest-neighbor presentation and the 2:1 aspect ratio; fullscreen can add black bars.

### 7.2. Higher dimensions require renderer changes

Upscaling a 512×256 texture is cheap but does not satisfy a request for higher **render** dimensions. The larger modes should allocate and render their final surfaces at the selected dimensions. Original bitmaps and selected effect simulation grids may remain at their authored resolution where that preserves appearance.

| Scene or routine | Concrete constraint found | High-resolution work |
| --- | --- | --- |
| [`mute95`](../cpp-offline/src/scenes/mute95_scene.cpp) | Fixed 512×256 warp/noise buffers and credit coordinates | Separate the logical effect grid from output sampling and overlay placement |
| [`domina`](../cpp-offline/src/scenes/domina_routine.cpp) | Fixed 512×256 indexed frame and 512-wide source strip | Preserve source scroll distance and speed; sample the authored strip into the selected output size |
| [`saari`](../cpp-offline/src/scenes/saari_scene.cpp) | Fixed camera half-width/height, sky dimensions, and terrain sampling coordinates | Parameterize projection, viewport sampling, reflection masks, and background sampling |
| [`kukot`](../cpp-offline/src/scenes/kukot_scene.cpp) | Fixed projection/particle scale and history allocation | Scale screen-space geometry and flares; resize histories and audit noise/flash indexing |
| [`maku`](../cpp-offline/src/scenes/maku_scene.cpp) | Fixed viewport constants and frame history, with some dynamic loops | Parameterize projection and effects coherently; preserve terrain texture dimensions |
| [`watercube`](../cpp-offline/src/scenes/watercube_scene.cpp) | Fixed packed output and a separate 256×256 ripple grid | Resize output/projection/layout; retain the ripple simulation grid initially |
| [`feta`](../cpp-offline/src/scenes/feta_scene.cpp) | Fixed packed output, feedback buffers, and frame history | Decide explicitly which feedback coordinates remain logical and which scale with output |
| [`uppol`](../cpp-offline/src/scenes/uppol_routine.cpp) | Fixed logical text layout and glyph placement | Scale layout and glyph presentation while preserving scroll duration |

Introduce one render configuration with output width/height and logical-to-output scale. Audit viewport constants separately from palette sizes, texture dimensions, and bit masks: replacing every occurrence of `256` would corrupt unrelated data structures.

Preserve camera field of view and scene framing. Scale pixel-sized blur distances, smear spans, panel widths, flare radii, and text positions according to their visual meaning. Keep simulation rates and authored noise seeds fixed. Increasing random draws per output pixel can change later RNG state; use logical noise fields or separate deterministic streams where needed.

A high-resolution mode can improve geometric edges while retaining pixelated original textures and effect fields. Document that choice. A mode that only stretches the entire 512×256 image should be labelled **display scale**, independently of **render resolution**.

### 7.3. Cost and performance gates

| Render dimensions | Pixels | Relative pixel count | One 32-bit surface | One upload per 50 Hz tick |
| --- | ---: | ---: | ---: | ---: |
| 512×256 | 131,072 | 1× | 0.5 MiB | 25 MiB/s |
| 1024×512 | 524,288 | 4× | 2 MiB | 100 MiB/s |
| 2048×1024 | 2,097,152 | 16× | 8 MiB | 400 MiB/s |

These are arithmetic lower bounds for surface storage and payload transfer, not browser throughput measurements. Six full-size 32-bit buffers would consume 3, 12, or 48 MiB respectively, before assets, audio, and GPU copies. Multi-pass pixel effects multiply memory traffic; geometry sorting does not necessarily scale with pixel count.

Measure each scene with disk output disabled. Record preparation time, render/update time, upload time, maximum catch-up backlog, long stalls, and peak memory. A useful initial acceptance target is p95 work below 16 ms per 20 ms simulation tick, with no sustained backlog; validate the complete sequence and transition peaks, not only an easy scene.

512×256 is the compatibility target. Release 1024×512 only after fidelity and timing pass. Treat 2048×1024 as optional and device-dependent. If pixel work dominates, first reuse scratch allocations and reduce copies; evaluate SIMD or selected GPU effects only after profiling. A wholesale shader rewrite would introduce a second fidelity project.

## 8. HTML entry page inspired by the original

Use [`original/forward/index.html`](../original/forward/index.html) as visual reference: its tiled `images/back.gif`, `images/aijja.gif` illustration, compact Arial typography, black text, credits, and numbered mode choices. Preserve the original files and author a new shell, for example under `web/`.

The entry page should offer three render choices, with 512×256 selected by default, followed by loading status and a Start button. Label 2048×1024 as experimental until validated. Keep the historical title, authorship, version/date as release provenance; describe the running edition as the C++/WebAssembly preservation port rather than repeating the original claim that it is Java.

Suggested flow:

```text
Intro artwork and credits
Render resolution: 512×256 / 1024×512 / 2048×1024
Preparing assets and music ... → Ready → Start
Canvas with Pause/Resume, Mute, Restart, Fullscreen
```

The old silent/low-resolution links need not become separate pages. A mute control can retain the same timeline. Keep debugging statistics behind a developer switch.

Set canvas backing dimensions explicitly to the chosen render dimensions. Do not silently multiply rendering resolution by `devicePixelRatio`; that would turn the highest mode into a much more expensive workload on a high-DPI display. By default, use matching numeric CSS dimensions, subject to available space. On small screens, fit the image while preserving aspect ratio and report the unchanged render dimensions. Fullscreen changes display size, not the rendering preset.

CSS pixels and physical display pixels are different on high-DPI screens. If strict physical pixel mapping is desired, implement it as a separate display policy using the measured device pixel ratio. It is not what the historical applet's `1x1` parameter controls.

Use a responsive layout retaining the original page's proportions at ordinary widths; do not constrain a 2048-wide canvas to the original 610-pixel table. Native HTML controls provide keyboard access and a reliable explicit start gesture.

## 9. WASM build, assets, and hosting

Add an Emscripten branch to CMake after the native host works. Compile the common core as C++11; apply SDL options only to the relevant host targets. Pin an emsdk release and the native SDL version in the build documentation.

Illustrative commands **after the proposed targets exist**:

```sh
emcmake cmake -S cpp-offline -B build/wasm -DCMAKE_BUILD_TYPE=Release
cmake --build build/wasm --target forward-web
```

Emscripten provides CMake toolchain integration. Use a separate build directory from the native compiler. [Emscripten build integration](https://emscripten.org/docs/compiling/Building-Projects.html).

The documented SDL2 option is `-sUSE_SDL=2`, needed during compilation and linking. Initially keep the application single-threaded and use prepared Web Audio playback; neither a pthread build nor a renderer worker is necessary for that design. [SDL2 build instructions](https://wiki.libsdl.org/SDL2/README-emscripten).

Replace the blocking application loop with `emscripten_set_main_loop_arg()` or an animation-frame callback that returns promptly. Asset and PCM preparation must also yield in bounded steps. An explicit state machine is preferable here to using Asyncify to retain the exporter loop. [Browser runtime and main loop](https://emscripten.org/docs/porting/emscripten-runtime-environment.html).

Stage only runtime assets and preload them into Emscripten's virtual filesystem, preserving paths such as `/original/forward/mods/jarnomix.xm`. With the virtual working directory at `/`, the current relative lookups can continue working. Audit filename case, especially when building on Windows. Preloading allows the existing synchronous C++ file readers to run after asynchronous download completes. [File packaging](https://emscripten.org/docs/porting/files/packaging_files.html).

Exclude historical `.class` files, `3DSRDR.exe`, reverse-engineering tools, reference captures, and exported movies from the web runtime package. Original source assets and their provenance remain in the repository.

Publish a static bundle containing the custom HTML/CSS, JavaScript loader/audio bridge, `.wasm`, and preloaded data. Serve it over HTTPS, or localhost during development, with the correct `application/wasm` type. Validate loading from a subdirectory and configure asset URLs accordingly. A double-clicked `file://` HTML page is not the supported launch path. The optional integrated worklet build additionally needs Cross-Origin-Opener-Policy and Cross-Origin-Embedder-Policy headers, with compatible resource responses. Verify `crossOriginIsolated` at startup for that build. [Emscripten shared-memory deployment](https://emscripten.org/docs/porting/pthreads.html).

Memory growth can simplify initial WASM bring-up, but allocate expected working storage before playback to avoid growth stalls. Reacquire JavaScript typed-array views after any WASM memory growth. Do not retain stale views of the heap, or count JavaScript/Web Audio storage as part of the WASM heap budget.

Use conservative optimization during parity validation. Audit integer overflow, signed shifts, float-to-int conversion, aliasing, and uninitialized state in packed-pixel and mixer code. Native success does not prove identical optimized WASM behavior. Avoid `-ffast-math` initially; record compiler versions and options with captures.

## 10. Validation and release criteria

The port needs two distinct proofs: equivalent deterministic content and stable real-time delivery.

| Check | Method | Pass condition |
| --- | --- | --- |
| Core extraction | Export fixed scene windows before/after refactoring at 512×256 | Identical event traces and PCM; identical pixels for unchanged code on the same toolchain |
| Mixer chunking | Render the same source with irregular block sizes and boundaries | Exact pre-device PCM and event timestamps compared with batch rendering |
| Continuous score | Trace module changes, joins, pre-show messages, and scene visibility | No duplicated/missing PCM spans; correct `0x1024`, `0x1230`, `0x1300`, and `0x1600` handling |
| Native versus WASM core | Capture framebuffers and PCM at matching source samples | Exact integer results where applicable; any floating-point image differences measured and reviewed |
| Presentation | Color-pattern test and captures of representative scenes | Correct color order, opaque output, orientation, dimensions, and aspect ratio |
| Synchronization | Log intended event sample and first displayed tick; measure audio/visual markers by loopback or external capture | No cumulative drift; internal quantization within one 20 ms tick, with actual display/audio latency separately measured |
| Real-time budget | Full run at each resolution, including transitions | No audible interruption or sustained simulation backlog on declared target hardware |
| Lifecycle | Repeated start/pause/resume/restart, hidden tab, fullscreen, audio interruption | No skipped event, unintended timeline advance, duplicate audio source, or retained scene history |
| Higher resolution | Compare framing/effects and downsampled captures against 512×256 | Same composition, event timing, scroll duration, and reviewed effect scale; pixel identity is not required |
| Deployment | Clean browser session, cold cache, asset failure, subdirectory hosting | Loading progress and actionable errors; all runtime files resolve without development tools |

Set an initial measured end-to-end A/V offset target of approximately 40 ms on representative built-in or wired outputs, and investigate any offset trend over the full run. This is a proposed release criterion, not a promise about arbitrary Bluetooth devices, browser compositors, or operating-system buffering. Deterministic event placement can be sample-accurate; visible output remains refresh-quantized.

Test current Chrome/Edge, Firefox, and Safari on desktop with recorded browser/OS versions and hardware. Exercise 60 Hz and at least one high-refresh display, and both common audio device rates. Add iOS Safari and Android Chrome at 512×256 as a separate mobile validation tier; do not infer mobile support from a desktop pass.

Use the existing Java captures for artistic review and the C++ exporter for transformation regressions. Remaining C++/Java fidelity differences must be recorded separately from regressions introduced by the real-time or WASM hosts. Store traces and capture parameters alongside results.

## 11. Work plan, estimates, and decision gates

Estimates assume one developer familiar with this repository, reuse of the current software renderer and mixer, and no broad repair of pre-existing visual differences. They represent focused working days, including validation.

| Stage | Deliverable and gate | Estimate |
| --- | --- | ---: |
| A. Freeze references and extract core | Offline output preserved; proposed host boundaries exist | 3–5 days |
| B. Continuous score and audio preparation | One score, exact module joins, resumable preparation and matching event/PCM traces | 5–9 days |
| C. Native SDL2 player | Full 512×256 playback, audio transport, pause/restart, profiling | 3–5 days |
| D. Browser host | Emscripten viewport, Web Audio clock, assets and lifecycle | 4–7 days |
| E. Intro page and release validation | Original-inspired shell, tested static package, desktop browser results | 5–8 days |
| F. True 1024×512 mode | Parameterized renderer, reviewed effect scales, full-run benchmark | Additional 8–15 days |
| G. Optional 2048×1024 mode | Memory/performance work and device qualification | Additional 5–12 days |
| Optional streaming audio | Worklet, bounded buffers, recovery and deployment validation | Additional 5–10 days |

The 512×256 browser release is therefore approximately **20–34 working days**, or **4–7 working weeks**, before contingency. True higher-resolution rendering is a separate substantial increment. Confirm these ranges after the first whole-demo native profile and browser audio prototype.

The critical gates are:

1. **Content gate:** core extraction and continuous playback preserve scene behavior and remove export-only joins.
2. **Audio gate:** startup time and whole-score memory fit the declared target devices; otherwise select streaming before release.
3. **Timing gate:** full native and browser runs maintain the agreed A/V tolerance and recover consistently from pauses.
4. **Resolution gate:** each larger mode passes every scene, not just framebuffer allocation and one successful screenshot.

The largest risks are continuous-score reconstruction, stateful effects under missed frames, and resolution-dependent effect behavior. Compiler bring-up and the HTML shell are comparatively contained tasks. The practical first implementation target is a complete, synchronized **512×256 native SDL2 player**, followed by the **512×256 browser edition**; promote 1024×512 and 2048×1024 only as their rendering and performance gates pass.
