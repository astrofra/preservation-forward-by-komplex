# Forward Mute95 Palette Investigation

## Objective

Verify whether the `mute95` intro mismatch could come from a palette or indexed-pixel decoding drift in `images/kosmos/krad3.gif`.

This was worth checking because the reference video stays mostly black around the centered `SAVIOUR` title, while the current desktop reconstruction can overfill the frame with blue energy.

## Runtime Path Under Test

The Java desktop build loads `krad3.gif` through the same AWT image-consumer path used by the original code:

- `mmaakma.majaKkA(URL)`
- `mmjamka`
- `kmajkka`

Relevant files:

- `java-desktop/src/main/java/mmaakma.java`
- `java-desktop/src/main/java/mmjamka.java`
- `java-desktop/src/main/java/kmajkka.java`
- `java-desktop/src/main/java/kmjjkmk.java`

## Probe Tooling

Two helper tools were added:

- `probe_krad3_palette_java_desktop.bat`
- `compare_krad3_palette.bat`

Supporting sources:

- `tools/java-src/ForwardPaletteProbe.java`
- `tools/compare_krad3_palette.py`

The Java probe dumps:

- runtime palette as CSV and raw `RGB` bytes
- runtime indexed pixel buffer as raw bytes
- runtime histogram of palette indices
- runtime preview PNG

The Python comparison then checks those runtime dumps against the raw first frame of `original/forward/images/kosmos/krad3.gif`.

## Result

Observed result from `documentation/reference-capture/palette-probe/compare/summary.txt`:

- `palette_diff_count=0`
- `pixel_index_diff_count=0`
- `histogram_diff_count=0`
- `palette_match=1`
- `indices_match=1`
- `histogram_match=1`

In other words:

- the runtime Java loader reconstructs exactly the same palette as the GIF file
- the runtime Java loader reconstructs exactly the same indexed pixel buffer as the GIF file
- there is no evidence here of a palette precision, palette ordering, or indexed-buffer corruption problem

## Conclusion

The `mute95` intro mismatch is **not** explained by a bad `krad3.gif` palette decode in the current desktop reconstruction.

This does **not** prove the intro is correct overall. It only rules out this specific hypothesis:

- the problem is not in the raw GIF palette values
- the problem is not in the raw GIF index buffer
- the problem is not in the `mmaakma -> mmjamka -> kmajkka` indexed image loading path

The remaining likely causes are still in the scene logic and pacing itself, especially:

- frame-driven accumulation rate in `kmjjkmk`
- warp/update cadence of the background field
- scene-time alignment of the intro effect relative to the script/music
