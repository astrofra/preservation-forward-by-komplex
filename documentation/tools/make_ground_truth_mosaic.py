#!/usr/bin/env python3
"""
Build a native-size ground-truth mosaic from `gt_img_*.png`.

Default use:
    python3 make_ground_truth_mosaic.py

The script:
- uses only `gt_...` images from img/figures;
- preserves every image at its native size, with no cropping and no resizing;
- chooses a rectangular grid whose overall silhouette stays reasonably close
  to 4:3 when possible, but may become square or more vertically elongated;
- writes each series label only once, on its first tile in the mosaic.

With the current 18 ground-truth images, the default heuristic selects a 3x6
grid, keeps all images, and produces a square overall mosaic.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

try:
    from PIL import Image, ImageDraw
except ImportError as exc:  # pragma: no cover - runtime dependency guard
    raise SystemExit(
        "Pillow is required. Install it with: python3 -m pip install Pillow"
    ) from exc

from make_portage_bands import (
    DEFAULT_FIGURES_DIR,
    DEFAULT_OUTPUT_DIR,
    GT_PREFIX,
    draw_outlined_text,
    find_font,
    fit_font,
    split_key,
    text_bbox,
)


DEFAULT_OUTPUT = DEFAULT_OUTPUT_DIR / "ground-truth-mosaic-4x3.png"
DEFAULT_TARGET_ASPECT = 4 / 3

Image.MAX_IMAGE_PIXELS = None


@dataclass(frozen=True)
class GroundTruthImage:
    key: str
    index: int
    series: str
    path: Path


@dataclass(frozen=True)
class GridCandidate:
    columns: int
    rows: int
    capacity: int
    empty_slots: int
    actual_aspect: float


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Build a native-size mosaic from ground-truth PNGs."
    )
    parser.add_argument(
        "--figures-dir",
        type=Path,
        default=DEFAULT_FIGURES_DIR,
        help=f"Directory containing the source PNGs (default: {DEFAULT_FIGURES_DIR})",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=DEFAULT_OUTPUT,
        help=f"Composite PNG to generate (default: {DEFAULT_OUTPUT})",
    )
    parser.add_argument(
        "--font",
        type=Path,
        default=None,
        help="Path to a Roboto TTF/OTF file. Auto-detected when omitted.",
    )
    parser.add_argument(
        "--background",
        default="#000000",
        help="Background color for the composite (default: #000000).",
    )
    parser.add_argument(
        "--target-aspect",
        type=float,
        default=DEFAULT_TARGET_ASPECT,
        help=(
            "Preferred overall mosaic aspect ratio, used only as a layout hint "
            f"(default: {DEFAULT_TARGET_ASPECT:.6f})."
        ),
    )
    return parser.parse_args()


def humanize_series(series: str) -> str:
    return series[:1].upper() + series[1:]


def extract_gt_images(figures_dir: Path) -> list[GroundTruthImage]:
    images = []
    for path in sorted(
        figures_dir.glob(f"{GT_PREFIX}*.png"),
        key=lambda candidate: split_key(candidate.stem[len(GT_PREFIX) :]),
    ):
        key = path.stem[len(GT_PREFIX) :]
        index, series = split_key(key)
        images.append(GroundTruthImage(key=key, index=index, series=series, path=path))
    if not images:
        raise SystemExit(f"No ground-truth images found in {figures_dir}")
    return images


def find_best_grid(
    image_count: int,
    source_aspect: float,
    target_aspect: float,
) -> GridCandidate:
    candidates: list[GridCandidate] = []

    for rows in range(1, image_count + 1):
        columns = (image_count + rows - 1) // rows
        capacity = rows * columns
        empty_slots = capacity - image_count
        actual_aspect = source_aspect * columns / rows
        candidates.append(
            GridCandidate(
                columns=columns,
                rows=rows,
                capacity=capacity,
                empty_slots=empty_slots,
                actual_aspect=actual_aspect,
            )
        )

    return min(
        candidates,
        key=lambda candidate: (
            candidate.actual_aspect > target_aspect,
            abs(candidate.actual_aspect - target_aspect),
            candidate.empty_slots,
            abs(candidate.columns - candidate.rows),
            candidate.rows,
        ),
    )


def build_mosaic(
    images: list[GroundTruthImage],
    output_path: Path,
    bold_font_path: Path,
    background: str,
    columns: int,
    rows: int,
) -> tuple[int, int]:
    sizes: list[tuple[int, int]] = []
    for image_info in images:
        with Image.open(image_info.path) as image:
            sizes.append(image.size)

    tile_width = max(width for width, _ in sizes)
    tile_height = max(height for _, height in sizes)
    outer_padding = max(18, round(min(tile_width, tile_height) * 0.08))
    gap = max(8, round(min(tile_width, tile_height) * 0.04))

    canvas_width = outer_padding * 2 + columns * tile_width + (columns - 1) * gap
    canvas_height = outer_padding * 2 + rows * tile_height + (rows - 1) * gap

    label_margin = max(10, round(tile_height * 0.05))
    label_stroke = max(2, round(tile_height * 0.012))
    label_font = fit_font(
        [humanize_series(image.series) for image in images],
        bold_font_path,
        max_size=max(18, round(tile_height * 0.14)),
        min_size=14,
        max_width=tile_width - 2 * label_margin,
        max_height=round(tile_height * 0.22),
        stroke_width=label_stroke,
    )

    canvas = Image.new("RGBA", (canvas_width, canvas_height), background)
    draw = ImageDraw.Draw(canvas)

    previous_series: str | None = None
    for position, image_info in enumerate(images):
        row = position // columns
        column = position % columns
        cell_x = outer_padding + column * (tile_width + gap)
        cell_y = outer_padding + row * (tile_height + gap)

        image = Image.open(image_info.path).convert("RGBA")
        image_x = cell_x + (tile_width - image.width) // 2
        image_y = cell_y + (tile_height - image.height) // 2
        canvas.alpha_composite(image, (image_x, image_y))

        if image_info.series != previous_series:
            label = humanize_series(image_info.series)
            text_width, text_height = text_bbox(
                label_font, label, stroke_width=label_stroke
            )
            text_x = image_x + label_margin
            text_y = image_y + image.height - text_height - label_margin
            if text_width > image.width - 2 * label_margin:
                text_x = image_x + max(0, (image.width - text_width) // 2)
            draw_outlined_text(
                draw,
                (text_x, text_y),
                label,
                label_font,
                label_stroke,
            )
        previous_series = image_info.series

    output_path.parent.mkdir(parents=True, exist_ok=True)
    canvas.save(output_path)
    return canvas_width, canvas_height


def main() -> None:
    args = parse_args()
    if args.target_aspect <= 0:
        raise SystemExit("--target-aspect must be greater than 0")

    figures_dir = args.figures_dir.expanduser().resolve()
    output_path = args.output.expanduser().resolve()

    if not figures_dir.exists():
        raise SystemExit(f"Figures directory not found: {figures_dir}")

    images = extract_gt_images(figures_dir)
    with Image.open(images[0].path) as sample:
        source_aspect = sample.width / sample.height

    grid = find_best_grid(
        image_count=len(images),
        source_aspect=source_aspect,
        target_aspect=args.target_aspect,
    )

    bold_font_path = find_font(
        None if args.font is None else args.font.with_name(args.font.name),
        ("Roboto-Bold.ttf", "Roboto-Bold.otf", "Roboto-Regular.ttf", "Roboto-Regular.otf"),
    )

    canvas_width, canvas_height = build_mosaic(
        images=images,
        output_path=output_path,
        bold_font_path=bold_font_path,
        background=args.background,
        columns=grid.columns,
        rows=grid.rows,
    )

    print(
        "Selected grid: "
        f"{grid.columns}x{grid.rows} ({len(images)} images, "
        f"{grid.empty_slots} empty slot(s)), "
        f"mosaic aspect {grid.actual_aspect:.3f}"
    )
    print(f"Canvas size: {canvas_width}x{canvas_height}")
    print(f"Wrote {output_path}")


if __name__ == "__main__":
    main()
