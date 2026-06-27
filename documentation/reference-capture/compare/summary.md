# Forward Capture Comparison

- Frames compared: 76
- Global mean absolute error: 73.43
- Global changed ratio: 71.86%

## Global Check

- Large full-frame mismatch across the run. Verify `--video-offset-ms` and the reference crop/scale filter before changing Java code.
- If the alignment is already correct, inspect: java-desktop/src/main/java/forward.java, java-desktop/src/main/java/kaaakka.java, java-desktop/src/main/java/mmaamma.java, java-desktop/src/main/java/kaajmma.java

## Scene Priorities

| Scene | Frames | Avg MAE | Avg Changed | Worst Capture | Suggested Focus |
|---|---:|---:|---:|---:|---|
| mute95 | 19 | 147.72 | 95.85% | 19 | timing / scene sequencing |
| maku | 6 | 98.00 | 83.94% | 52 | timing / scene sequencing |
| watercube | 6 | 78.41 | 72.37% | 58 | timing / scene sequencing |
| feta | 6 | 67.88 | 84.85% | 64 | timing / scene sequencing |
| saari | 15 | 47.90 | 75.03% | 25 | timing / scene sequencing |
| domina | 5 | 40.57 | 56.90% | 20 | palette / fade / post effect |
| uppol | 6 | 18.46 | 53.10% | 70 | scene-specific render path |
| kukot | 12 | 22.78 | 38.56% | 45 | scene-specific render path |
| unknown | 1 | 3.04 | 1.94% | 0 | camera / raster / local effect |

## Suggested Actions

### mute95
- Diff is mostly structural (`changed_ratio` 95.85%). Check scene activation timing, message handling, and time-driven state in `java-desktop/src/main/java/kmjjkmk.java`.
- Scene average: MAE 147.72, changed ratio 95.85%.

### maku
- Diff is mostly structural (`changed_ratio` 83.94%). Check scene activation timing, message handling, and time-driven state in `java-desktop/src/main/java/kmjjmka.java`.
- Scene average: MAE 98.00, changed ratio 83.94%.

### watercube
- Diff is mostly structural (`changed_ratio` 72.37%). Check scene activation timing, message handling, and time-driven state in `java-desktop/src/main/java/kmajmka.java`.
- Scene average: MAE 78.41, changed ratio 72.37%.

### feta
- Diff is mostly structural (`changed_ratio` 84.85%). Check scene activation timing, message handling, and time-driven state in `java-desktop/src/main/java/kmaamka.java`.
- Scene average: MAE 67.88, changed ratio 84.85%.

### saari
- Diff is mostly structural (`changed_ratio` 75.03%). Check scene activation timing, message handling, and time-driven state in `java-desktop/src/main/java/maajmka.java`.
- Scene average: MAE 47.90, changed ratio 75.03%.

### domina
- Brightness or fade drift dominates (`|mean_luma_delta|` 32.9). Inspect blend/fade/palette logic in `java-desktop/src/main/java/kajakka.java` and the shared framebuffer code.
- Scene average: MAE 40.57, changed ratio 56.90%.

### uppol
- Mixed mismatch pattern. Start with `java-desktop/src/main/java/mmaakmk.java` and compare against the worst capture of the scene.
- Scene average: MAE 18.46, changed ratio 53.10%.

### kukot
- Mixed mismatch pattern. Start with `java-desktop/src/main/java/kajjkka.java` and compare against the worst capture of the scene.
- Scene average: MAE 22.78, changed ratio 38.56%.

### unknown
- Mixed mismatch pattern. Start with `java-desktop/src/main/java/forward.java` and compare against the worst capture of the scene.
- Secondary files: java-desktop/src/main/java/kaaakka.java, java-desktop/src/main/java/mmaamma.java, java-desktop/src/main/java/kaajmma.java
- Scene average: MAE 3.04, changed ratio 1.94%.
