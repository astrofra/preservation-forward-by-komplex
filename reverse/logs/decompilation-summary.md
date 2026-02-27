# Decompilation Summary (Single-Class Regeneration)

- Input classes: `87` (`original/forward/**/*.class`)
- Strategy: run CFR one class at a time with:
  - `--usenametable false --antiobf true --lenient true`
- CFR results:
  - `86` classes fully decompiled
  - `1` class with a failed method: `muhmu/hifi/device/DeviceMSbase.class`

## Fallback Applied

- Decompiler fallback: `procyon-decompiler`
- Target class: `muhmu/hifi/device/DeviceMSbase.class`
- Final output replaced in:
  - `reverse/cfr_single/muhmu/hifi/device/DeviceMSbase.java`
- Original CFR output kept as backup:
  - `reverse/cfr_single/muhmu/hifi/device/DeviceMSbase.cfr_failed.java`

## Final State

- Final tree: `reverse/cfr_single`
- Remaining `"This method has failed to decompile"` markers in final `.java` files: `0`
- Per-class status table:
  - `reverse/logs/cfr_single_status.tsv`
