# Regression Workspace

This directory is the working area for parity checks against the declared Java baseline.

Suggested layout:

```text
documentation/
  regression/
    baseline/
    candidate/
    compare/
      frame_metrics.csv
      summary.md
      visuals/
```

Recommended use:

- `baseline/`: copies or links to the frozen reference-capture set used for a specific comparison run
- `candidate/`: the output being tested, for example a renamed Java branch or the C++11 exporter
- `compare/`: generated metrics, manifests, visual diffs, and short human-readable summaries

This folder is only scaffolded here. No automated comparison tool has been added yet.
