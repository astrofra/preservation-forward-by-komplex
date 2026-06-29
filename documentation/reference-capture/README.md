# Reference Capture Baseline

## Status

On June 29, 2026, the current project release was declared the Java behavioral baseline.

The first frozen baseline snapshot is now stored under:

- `documentation/reference-capture/baseline-java-2026-06-29/`

That declaration means:

- `java-desktop` is the reference implementation for parity work
- future C++11 exporter validation should compare against Java captures from this baseline
- broad Java renaming should not change behavior without regression evidence

## Intended Contents

This directory holds frozen capture artifacts produced from declared Java baselines.

Suggested layout:

```text
documentation/
  reference-capture/
    baseline-java-2026-06-29/
      manifest.csv
      frames/
      notes.md
```

The working path:

- `documentation/reference-capture/java/`

can still be used for fresh or ad hoc self-captures, but the dated baseline directory is the frozen reference for parity work.

## Capture Policy

- Keep the baseline tied to a named release or dated checkpoint.
- Preserve the exact launch parameters used for capture.
- Prefer native framebuffer captures over presentation-scaled screenshots.
- Store scene and timing metadata alongside the frames.

## Current Baseline Snapshot

The current frozen snapshot contains:

- `manifest.csv`
- `frames/`
- `notes.md`
