from __future__ import annotations

import argparse
import csv
from pathlib import Path

from PIL import Image


def load_runtime_palette(path: Path) -> list[tuple[int, int, int]]:
    data = path.read_bytes()
    if len(data) != 256 * 3:
        raise ValueError(f"Unexpected runtime palette size: {len(data)}")
    return [tuple(data[index:index + 3]) for index in range(0, len(data), 3)]


def load_runtime_indices(path: Path) -> bytes:
    return path.read_bytes()


def load_gif_palette_and_indices(path: Path) -> tuple[list[tuple[int, int, int]], bytes, tuple[int, int]]:
    image = Image.open(path)
    if image.mode != "P":
        image = image.convert("P")
    palette = image.getpalette()
    if palette is None:
        raise ValueError(f"No palette found in {path}")
    rgb = [tuple(palette[index:index + 3]) for index in range(0, 256 * 3, 3)]
    return rgb, image.tobytes(), image.size


def histogram(indices: bytes) -> list[int]:
    counts = [0] * 256
    for value in indices:
        counts[value] += 1
    return counts


def write_palette_diffs(output: Path, raw_palette: list[tuple[int, int, int]], runtime_palette: list[tuple[int, int, int]]) -> int:
    diff_count = 0
    with output.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["index", "raw_red", "raw_green", "raw_blue", "runtime_red", "runtime_green", "runtime_blue"])
        for index, (raw_rgb, runtime_rgb) in enumerate(zip(raw_palette, runtime_palette)):
            if raw_rgb != runtime_rgb:
                diff_count += 1
                writer.writerow([index, *raw_rgb, *runtime_rgb])
    return diff_count


def write_histogram(output: Path, raw_histogram: list[int], runtime_histogram: list[int]) -> None:
    with output.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["index", "raw_count", "runtime_count", "delta"])
        for index, (raw_count, runtime_count) in enumerate(zip(raw_histogram, runtime_histogram)):
            writer.writerow([index, raw_count, runtime_count, runtime_count - raw_count])


def main() -> int:
    parser = argparse.ArgumentParser(description="Compare krad3.gif raw palette/indices against runtime Java loader output.")
    parser.add_argument("--gif", required=True, type=Path)
    parser.add_argument("--runtime-dir", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    args = parser.parse_args()

    gif_path = args.gif.resolve()
    runtime_dir = args.runtime_dir.resolve()
    output_dir = args.output_dir.resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    raw_palette, raw_indices, raw_size = load_gif_palette_and_indices(gif_path)
    runtime_palette = load_runtime_palette(runtime_dir / "runtime_palette.rgb")
    runtime_indices = load_runtime_indices(runtime_dir / "runtime_indices.bin")

    if len(raw_indices) != len(runtime_indices):
        raise ValueError(f"Index length mismatch: raw={len(raw_indices)} runtime={len(runtime_indices)}")

    palette_diff_count = write_palette_diffs(output_dir / "palette_diffs.csv", raw_palette, runtime_palette)
    raw_histogram = histogram(raw_indices)
    runtime_histogram = histogram(runtime_indices)
    write_histogram(output_dir / "histogram_compare.csv", raw_histogram, runtime_histogram)

    pixel_diff_count = sum(1 for left, right in zip(raw_indices, runtime_indices) if left != right)
    histogram_diff_count = sum(1 for left, right in zip(raw_histogram, runtime_histogram) if left != right)

    with (output_dir / "summary.txt").open("w", encoding="utf-8") as handle:
        handle.write(f"gif_path={gif_path}\n")
        handle.write(f"runtime_dir={runtime_dir}\n")
        handle.write(f"image_width={raw_size[0]}\n")
        handle.write(f"image_height={raw_size[1]}\n")
        handle.write(f"palette_diff_count={palette_diff_count}\n")
        handle.write(f"pixel_index_diff_count={pixel_diff_count}\n")
        handle.write(f"histogram_diff_count={histogram_diff_count}\n")
        handle.write("palette_match=" + ("1" if palette_diff_count == 0 else "0") + "\n")
        handle.write("indices_match=" + ("1" if pixel_diff_count == 0 else "0") + "\n")
        handle.write("histogram_match=" + ("1" if histogram_diff_count == 0 else "0") + "\n")

    print(f"palette_diff_count={palette_diff_count}")
    print(f"pixel_index_diff_count={pixel_diff_count}")
    print(f"histogram_diff_count={histogram_diff_count}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
