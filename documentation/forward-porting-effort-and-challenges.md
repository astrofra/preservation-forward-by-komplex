# Forward (Komplex) Porting Effort and Challenges

## 1) Scope and Source Material

This assessment is based on:

- `original/forward` (original Java release payload: classes, HTML launchers, assets, XM modules)
- `original/forward.nfo`
- `original/pouet/forward by Komplex __ pouët.net.html` (saved Pouet page and metadata)

No assumptions were made from modern browser compatibility, only from shipped files and archived scene metadata.

## 2) Historical Context (from shipped files + Pouet snapshot)

- Prod: **forward** by **Komplex**
- Party: **The Gathering 1998**
- Result: **1st** in Java demo compo
- Shipped release notes mention `v1.02` and compatibility fixes dated **April 26, 1998** (`version.txt`)
- Landing page advertises:
  - "Unlimited 44kHz 16bit stereo audio engine"
  - "28 channel XM music"
  - "True 32bit MuhmuColor"
  - "Realtime 3D reflection engine"
  - "MuhmuScript 2.0 demo language"

Pouet comments also reinforce that runtime behavior and sound quality differed by JVM/browser combination (for example, users recommending `jview` instead of browser embedding on Windows for better audio).

## 3) What the Java Release Tells Us Technically

### Runtime model

- Applet-based (`<applet code="forward" width="512" height="256">`)
- Explicit old JVM targets in docs: IE3/IE4, Netscape 4, JDK 1.0.2/1.1.x era
- Launch options:
  - `nosound 1`
  - `1x1 1` (high-res mode)
- `README.TXT` states appletviewer was insufficient memory (<16 MB heap)

### Binary/code layout

- `79` root `.class` files + `8` classes under `muhmu/hifi/device`
- Total root class payload: `256,720` bytes
- Main class file reports Java class version `45.3` (JDK 1.1 era)

### Assets/layout

- Modules: `mods/jarnomix.xm`, `mods/kuninga.xm` (FastTracker II XM)
- Meshes: `.igu` text files (`meshes/*.igu`) and source/export artifacts (`asses/*.ase`, `meshes/3DSRDR.exe`)
- Texture/media assets under `images/` and subfolders

### Strong subsystem clues from class strings

- `forward.class`: applet lifecycle, script-like timeline commands (`show`, `msg`, `init`, `shutdown`)
- `majjmka.class`: XM/MOD loader strings (`loading XM`, `loading MOD`, `Invalid module`, `Extended Module`)
- `kmajkka.class`, `majakmk.class`, `mmaamma.class`, `mmjamka.class`: software image pipeline using `MemoryImageSource`, `DirectColorModel`, `IndexColorModel`, `ImageConsumer`
- `muhmu/hifi/device/*`: platform-specific audio backends:
  - IE3/IE4 DirectSound via `com/ms/directX/*`
  - `sun.audio` backend
  - no-sound fallback device

This strongly indicates a CPU software renderer + custom timeline + custom module playback stack, with AWT/Applet used mainly for hosting/input/presentation/audio plumbing.

## 4) Decompilation Status and CFR Workflow

### Current status in this workspace

- `cfr` is not installed
- `javap` could not run because no Java runtime is configured on this machine

### Recommended CFR workflow

Once a JRE/JDK is available, run:

```bash
mkdir -p reverse/cfr
java -jar tools/cfr.jar original/forward/forward.class --outputdir reverse/cfr --silent true
java -jar tools/cfr.jar original/forward/*.class --outputdir reverse/cfr --silent true
java -jar tools/cfr.jar original/forward/muhmu/hifi/device/*.class --outputdir reverse/cfr --silent true
```

Then immediately build a symbol map:

- Obfuscated class name -> inferred role (renderer, script parser, XM loader, mesh parser, app framework, audio backend).

## 5) Porting Approach to C++

## Recommendation

Port to a **CPU-rendered C++ core** and treat display/audio/window/input as host layers.

- Keep original software rendering logic semantics.
- Replace Applet/AWT glue with SDL2 (and optionally OpenGL only for final blit/upscale).

### Why SDL2-first over OpenGL-first

- Your core is software-rendered already; GPU rasterization is not needed to be faithful.
- SDL2 streaming textures give a simple, deterministic path for presenting a CPU frame buffer.
- Optional OpenGL can be added later for CRT/post effects, scaling filters, and fullscreen presentation quality.

### Practical host architecture

- `core/`:
  - timeline/script system (MuhmuScript-like behavior)
  - scene state
  - software raster/effects
  - resource loaders (`.igu`, possibly `.ase` fallback)
- `platform/`:
  - SDL2 window/input/timer
  - audio callback device
  - frame upload (`SDL_UpdateTexture` + `SDL_RenderCopy`)
- `audio/`:
  - XM player integration layer

## 6) XM Music Strategy

You have three viable paths:

1. `libxmp` integration (recommended MVP)
- Pros: mature XM support, simple embedding, fast bring-up
- Cons: may not match original mixer quirks 1:1

2. `libopenmpt` integration
- Pros: excellent format support and stable API
- Cons: larger dependency, still may differ from 1998 playback behavior

3. Port the original Java module replayer (`majjmka` + related mixer classes)
- Pros: highest authenticity potential
- Cons: highest reverse-engineering effort/risk

Recommended sequence:

- Start with `libxmp` to unblock sync and timeline iteration.
- If A/B comparison shows audible drift vs captures, schedule a second phase to port original replayer logic.

## 7) Main Challenges

1. Heavy symbol obfuscation in most classes.
2. Legacy AWT/Applet event model must be replaced with modern input/window lifecycle.
3. Legacy audio backends are Windows IE DirectX and `sun.audio`; both need full replacement.
4. Potential behavior coupling between script timing and audio buffer timing.
5. Asset pipeline uncertainty (`.igu`/`.ase` both present; runtime path must be confirmed via decompilation).
6. Deterministic timing differences on modern high-refresh systems.
7. Legal/IP clarity for redistribution and derivative code (a class string even warns about decompilation licensing).

## 8) Effort Estimate

Single experienced engineer, full-time:

- **MVP playable port** (visuals + music + timeline, not pixel-perfect): **3 to 5 weeks**
- **Faithful preservation port** (close behavior match, tuning + validation): **6 to 10 weeks**

Rough phase split:

1. Reverse engineering + class role mapping: 4 to 8 days
2. Engine scaffolding (CMake, SDL2, frame loop, input): 2 to 4 days
3. Renderer/effects port: 8 to 15 days
4. XM integration + sync: 3 to 8 days
5. Loader/timeline parity + polish/testing: 6 to 15 days

## 9) Suggested Execution Order

1. Set up JDK + CFR; decompile all classes.
2. Produce a class responsibility map and call graph centered on `forward`.
3. Build a C++ harness showing a CPU frame buffer at 512x256 through SDL2.
4. Port script/timeline interpreter and scene switching first (before polishing effects).
5. Integrate `libxmp` for immediate audio.
6. Port renderer/effects and compare against capture references (Pouet screenshot + available video captures).
7. Decide whether original Java XM mixer port is needed for authenticity.
8. Freeze deterministic timing model and ship a preservation build.

## 10) Bottom Line

This is a feasible preservation port. The project risk is not "can modern APIs display it" (they can), but "how much original behavior fidelity you want."

- For a practical revival, SDL2 + software framebuffer + libxmp is the fastest path.
- For scene-accurate preservation, budget significant time for deobfuscation and timing/audio parity work.
