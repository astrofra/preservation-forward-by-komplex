---
name: preserve-java-demoscene-artworks
description: Long-term preservation workflow for demoscene artworks written in Java. Use when restoring bytecode-only releases, dealing with obfuscation, repairing decompiled Java, migrating applets to desktop hosts, packaging standalone Windows builds with an embedded Java runtime, deobfuscating selectively, porting to C or C++ for real-time or offline preservation targets, generating semi-automated visual and audio quality-control feedback, and documenting the full preservation process for durable archival reuse.
---

# Preserve Java Demoscene Artworks

Preserve the artifact before optimizing the codebase.

Treat long-term preservation as a `50+ year` problem:

- preserve the original release
- preserve the reverse-engineering evidence
- preserve a rebuildable source form
- preserve a runnable desktop form
- preserve a dependency-light native form when justified
- preserve automated validation outputs
- preserve the reasoning that explains why each transformation was made

Do not assume the current repository layout exists in future uses of this document. Use neutral directory and artifact names in the preservation workflow, then adapt them to the local project.

## Core Rules

- Freeze behavior before broad cleanup.
- Keep `observed behavior`, `inferred meaning`, and `chosen names` separate.
- Prefer source-faithful repair over premature redesign.
- Preserve original asset paths and data formats unless migration is itself documented and validated.
- Make every risky change replayable through automated captures and manifests.
- Prefer open, inspectable outputs such as `PNG`, `TGA`, `WAV`, `CSV`, and plain-text notes.
- Avoid preservation plans that require browsers, plugins, network services, or dead runtimes.
- Keep the workflow portable across repositories and toolchains.

## Recommended Preservation Layers

Keep the project split into explicit layers. Use local names if needed, but preserve the separation:

- `original artifact`
- `raw reverse-engineering outputs`
- `repaired Java reconstruction`
- `desktop-hosted Java build`
- `native preservation port`
- `reference captures and regression outputs`
- `process documentation and symbol maps`

Do not collapse raw evidence, repaired source, and speculative cleanup into one tree.

## Decompile the Java Bytecode

Do not trust a single decompiler.

Use at least two decompilers and keep their outputs separate. Add a third tool only when the first two disagree in critical regions.

Recommended bytecode recovery procedure:

1. Preserve the original `.class`, `jar`, `html`, `txt`, and asset files unchanged.
2. Decompile class-by-class with multiple tools.
3. Keep the raw outputs under tool-specific folders.
4. Build the repaired working tree from the best file or method candidate, not from blind bulk output.
5. Inspect broken methods with `javap -c -p` whenever decompiled control flow still contains `GOTO` artifacts, label spaghetti, or suspicious casts.
6. Record which source won for each repaired class.

Treat decompilation artifacts as evidence, not truth.

## Handle Obfuscation Without Lying to Yourself

Assume obfuscated names carry little or no semantic value.

In Java demoscene releases, obfuscation often introduces:

- meaningless class and method names
- case-only name distinctions
- collapsed ownership boundaries
- string-based reflection traps
- dense scene-local scratch state that compiles but is not self-explanatory

Follow these rules:

- Treat names as hypotheses until supported by structure, assets, scripts, or runtime behavior.
- Maintain a symbol map with `legacy symbol`, `proposed name`, `kind`, `confidence`, `evidence`, and `status`.
- Use structured refactors only. Never do broad search-and-replace on case-sensitive obfuscated code.
- Update reflection strings and string-based class references in the same patch.
- Keep legacy-name comments during the transition period when traceability still matters.

Good evidence sources for deobfuscation:

- scene registry and timeline scripts
- asset filenames and directory structure
- loader behavior
- render and mixer call graphs
- capture manifests
- side-by-side runtime observation

Deobfuscate in this order:

1. scene classes and top-level orchestration
2. obvious lifecycle methods
3. shared math, render, loader, and mixer types
4. shared engine fields
5. scene-private state only after effect understanding is strong

Do not rename scene-local accumulators just to make the tree look clean.

## Repair the Decompiled Java Tree

Create a dedicated reconstruction tree and keep raw reverse outputs immutable.

Repair only what is needed to restore source integrity:

- broken loops and branches
- failed `switch` reconstruction
- bad numeric typing
- illegal casts
- dead API references
- missing helper types

Prefer a `minimal semantic repair` mindset:

- preserve control flow if it can be proven
- rewrite only the method fragments that the decompiler failed to structure
- avoid mixing readability cleanup with behavioral repair

Treat the repaired Java tree as the first preservation-grade source form.

## Port the Runtime from Applet to Desktop

Do not rewrite the whole engine just because the original host was an applet.

Replace the dead hosting layer with a thin desktop compatibility layer:

- replace `Applet` with a desktop-hosted surface such as `Panel`, `Canvas`, or a minimal window host
- replace `AppletStub` with a parameter and base-URL holder
- replace `AppletContext` with a small desktop context shim
- preserve parameter parsing semantics where that affects behavior
- preserve `getCodeBase()` and `getDocumentBase()` style asset resolution when the original code depends on them

The correct preservation pattern is:

- keep the original engine structure
- inject a runtime `basedir` when needed
- avoid a full resource-loader rewrite unless the original lookup logic is unrecoverable
- make packaged and source builds resolve the same asset families

If the original release assumed a browser working directory, replace that assumption explicitly rather than implicitly.

## Port to a Recent JDK

Modernize only the runtime assumptions that are dead or unstable.

Typical Java demoscene repairs:

- replace `sun.audio`
- replace vendor-specific or browser-bound audio paths
- replace browser lifecycle dependencies
- keep a `nosound` path for regression smoke tests
- compile on a current LTS JDK such as `17+`

Keep the build simple when preservation is the goal. Prefer a direct, inspectable `javac` workflow unless a larger build system solves a real preservation problem.

Avoid introducing Maven, Gradle, or framework-heavy packaging unless they materially improve reproducibility or portability.

## Repair Timing Before Chasing Cosmetic Fidelity

Many late-1990s Java demos relied on frame-driven behavior that breaks on uncapped modern machines.

When modern hardware causes pacing drift:

- identify which subsystems were authored as frame-driven
- separate frame-driven logic from genuinely time-driven logic
- normalize fragile updates to a documented virtual cadence such as `50 Hz`
- validate against captures rather than intuition

Typical failure modes:

- scrolls outrun music
- palette or noise buildup saturates too early
- scene-local oscillators spin too fast
- water, warp, or damping logic loses the original cadence

Fix timing with restraint. Do not turn every effect into a new engine architecture.

## Build a Standalone Windows `.exe` with Embedded Java Runtime

Prefer `jpackage` for long-term maintainability on Windows.

Recommended packaging pattern:

1. recompile the repaired Java tree
2. build a runnable jar
3. stage the runtime assets into the package input
4. provide a launcher that resolves the packaged asset root
5. convert or generate the application icon for Windows
6. run `jpackage --type app-image`
7. optionally build an installer only if the installer toolchain is available

Packaging rules:

- make the `app-image` the primary deliverable
- treat the installer as optional distribution sugar
- keep the package runnable without a separately installed JDK
- ship editable launch configuration when it affects display modes or compatibility
- copy provenance notes such as original readme and version metadata into the package

Validate the packaged build on a clean Windows machine with no developer checkout and no preinstalled JDK.

## Decide When Deobfuscation Is Worth It

Do not start with large-scale renaming.

Deobfuscation becomes worth doing when it improves one of these:

- bug fixing
- parity debugging
- porting confidence
- maintenance of the repaired Java baseline
- preservation documentation quality

Use rename-only passes with strict boundaries:

- one subsystem at a time
- no behavioral edits in the same commit
- compile and capture after every batch

If the goal is a native port, do not block the port on a fully cleaned Java tree. A confidence-based symbol map is often enough.

## Choose C or C++ Based on Source Shape, Not Ideology

Use the repaired Java tree as the native-port source of truth whenever possible.

Choose `C++11` or a similarly conservative subset when the Java source is strongly object-centric:

- abstract scene families
- polymorphic dispatch
- stateful surface and renderer types
- layered audio and mixer objects
- loader classes with stable ownership boundaries
- a long-lived native codebase is expected to stay portable across `Windows`, `Linux`, and `macOS`

Choose `C99` only when one of these is true:

- the target is deliberately procedural
- the code can be flattened without destroying parity work
- the first native milestone is a headless exporter rather than an interactive player

For the `C++` path, design portability in from the start:

- keep the core runtime free of platform GUI and audio APIs
- isolate filesystem and path utilities behind a tiny platform layer
- prefer portable build systems such as `CMake`
- avoid platform-specific assumptions in path handling, launch flow, and packaging logic
- treat `Windows`, `Linux`, and `macOS` as first-class preservation targets even if one platform is used as the initial development host

In practice:

- `C++` is usually the better fit for a source-shaped port
- `C` is often more suitable for a deliberately minimal offline exporter

## Prefer an Offline Exporter Before a Real-Time Native Player

For preservation, an offline exporter is usually the lowest-risk first native target.

It removes the need for:

- a windowing API
- a live audio device
- input handling
- browser or desktop hosting
- platform-specific timing loops

Keep the first native target deterministic:

- fixed frame rate such as `50 fps`
- fixed audio rate such as `22050 Hz`
- one integer master timeline
- `TGA` or `PNG` frame sequence
- one `WAV` file
- external `ffmpeg` wrappers for muxing

When audio drives timing, prefer the audio sample position as the master clock. `22050 / 50 = 441` samples per frame is an example of a clean preservation-friendly ratio.

## Keep Native Dependencies Minimal and Explicit

Do not rebuild dead platform dependencies in the native port.

Accept a vendored, source-level third-party decoder when it reduces long-term risk without creating install-time runtime dependencies:

- checked into the repository
- license kept with the source
- no package manager requirement at runtime
- narrow purpose

Keep proprietary-format loaders source-shaped and in-project:

- `.xm` / `.mod`
- custom mesh formats
- scene or animation formats

Preserve the original assets as runtime inputs when possible. Do not require a Java preprocessing step in the native runtime path unless absolutely necessary.

## Watch for Renderer Parity Traps

Do not reduce every 3D mismatch to `normals`, `UVs`, or `blend mode`.

Old Java demoscene pipelines often rely on scene-specific renderer contracts that are easy to break during reconstruction or porting.

Common trap families:

- `geometry semantics`: axis meaning, height interpretation, mirrored clones, out-of-bounds fallback rules
- `visibility semantics`: custom culling, explicit face-test signs, concave-mesh face selection, depth-sort instead of z-buffer semantics
- `material semantics`: `opaque`, `additive`, masked additive, palette-driven composition, single-use reflection or water masks
- `texture semantics`: procedural UVs, implicit mapping, post-load UV tweaks, env-map matrix tricks, split texture usage inside one asset
- `raster semantics`: affine vs projective interpolation, fixed-point overflow, truncation rules, interpolant seeding, software-only edge behavior
- `camera semantics`: matrix build order, track-vs-target orientation rules, per-scene transform hacks, clone-specific transform state

Use this diagnosis order:

1. classify the symptom before editing code
2. decide whether the issue is geometric, visibility-related, compositing-related, raster-related, or timing-related
3. inspect the scene-specific Java renderer contract before applying generic graphics-engine intuition
4. change one contract at a time and re-capture

Useful sanity questions:

- Does this really indicate broken normals, or is it ordering or culling?
- Is this material globally additive, or additive only through a local mask?
- Are these UVs authored, procedural, or reconstructed at runtime?
- Does the reflected clone inherit every matrix and env-map tweak?
- Is the scene truly z-buffered, or only depth-sorted?
- Is the visible mismatch in geometry generation, composition, or rasterization?

Treat `looks wrong in 3D` as a symptom class, not as a diagnosis.

## Generate Semi-Automated Visual Feedback

Use frame capture as the core regression oracle.

The minimal pattern is:

- the Java baseline writes `manifest.csv`
- captures store scene and timing metadata
- frozen baselines live under dated folders

Follow this workflow:

1. Freeze a dated Java baseline.
2. Capture candidate outputs from the repaired Java tree or the native exporter.
3. Align by manifest metadata first, not by frame index alone.
4. Compare images automatically.
5. Generate human-review artifacts for the worst mismatches.

Minimum visual outputs:

- `frame_metrics.csv`
- per-scene summary
- side-by-side images
- boosted absolute diff images
- top-N failure list

Useful visual metrics:

- exact equality where possible
- MAE
- RMSE
- PSNR
- SSIM

Use external reference videos carefully:

- good for scene triage
- weak as a pixel-perfect oracle if the source is lossy, cropped, scaled, or offset

## Generate Semi-Automated Audio Feedback

Treat audio comparison as a second oracle alongside visual parity.

For native exporters, generate a `WAV` file for every candidate build and keep timing metadata beside it.

Recommended automated audio outputs:

- waveform overlay or alignment plot
- cross-correlation delay estimate
- RMS and peak statistics
- clipping and DC-offset report
- spectrogram comparison images
- onset or event timing report
- song-position or pattern-order trace when the player exposes it

Recommended audio comparison rules:

- compare uncompressed `WAV` whenever possible
- compare against native module-rendered reference audio before comparing against lossy video audio
- if only video audio exists, use the result as triage, not proof
- correlate audio events with scene transitions and manifest timestamps

Recommended QC directory shape:

```text
regression/
  baseline/
  candidate/
  compare/
    frame_metrics.csv
    audio_metrics.csv
    summary.md
    visuals/
    spectrograms/
```

Keep the system semi-automatic. Let scripts rank the failures, but let a human decide whether the artifact is still artistically faithful.

## Document the Process as Part of the Preservation Object

Do not treat documentation as an afterthought.

Preserve:

- the reconstruction process
- the packaging workflow
- the capture workflow
- the deobfuscation policy
- the symbol map
- the native port rationale
- the exact baseline dates and parameters
- the toolchain versions used for each declared release

At minimum, keep these document types:

- reconstruction log
- symbol map
- baseline declaration note
- packaging note
- regression workflow note
- port feasibility note
- scene- or subsystem-specific investigation notes where parity work is non-trivial

For every difficult repair, record:

- the original failing artifact
- the chosen evidence
- whether `javap` or another bytecode inspection step was required
- whether the change was behavioral, structural, or purely lexical

## Define the Long-Term Deliverables

For a serious preservation package, aim to keep all of these:

- original release files
- raw decompiler outputs
- repaired Java source
- desktop build scripts
- packaged Windows `app-image`
- native exporter source and scripts
- reference captures and manifests
- regression outputs and summaries
- symbol map and process documentation
- vendored third-party source dependencies with licenses

If one layer becomes unusable in the future, the next layer should still remain actionable.

## Default Execution Order

1. Freeze the original artifact.
2. Decompile with multiple tools.
3. Rebuild a minimal compiling Java tree.
4. Replace dead hosting and audio dependencies.
5. Validate a modern desktop run.
6. Freeze a dated Java behavioral baseline.
7. Package a standalone Windows build with embedded runtime.
8. Deobfuscate selectively when it unlocks maintenance or porting.
9. Port to a deterministic native exporter.
10. Use visual and audio QC to compare every major step.
11. Document every irreversible or evidence-based decision.

## Repository Adaptation Note

When applying this workflow to a specific repository, add a short local mapping note that explains where each preservation layer lives.

Example mapping:

```text
original artifact              -> original/
raw reverse outputs            -> reverse/
repaired Java reconstruction   -> java-desktop/
native preservation port       -> cpp-offline/
baseline captures              -> documentation/reference-capture/
regression workspace           -> documentation/regression/
```

Keep that mapping outside the core workflow so the document remains usable in other repositories.

## Bottom Line

Preserve the work in layers.

For Java demoscene artifacts, the most durable path is usually:

- original artifact preserved unchanged
- repaired Java reconstruction preserved as the first readable source form
- desktop host preserved as the first widely runnable form
- standalone Windows package preserved as the easiest distribution form
- native exporter preserved as the lowest-dependency long-term fallback
- documentation and regression evidence preserved as the proof that fidelity was not guessed
