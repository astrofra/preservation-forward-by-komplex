#!/usr/bin/env python3
"""
Assemble two horizontal image bands comparing ground truth to the C++ port.

Default use:
    python3 make_cpp_port_bands.py

This builds one composite PNG with:
- either a single two-row strip;
- or, by default, two stacked half-strips arranged as:
  GT / C++ port / GT / C++ port.

The C++ images are paired by alphanumeric order starting from a configurable
first file. Series labels (`mute95`, `saari`, etc.) come from the ground-truth
filenames and are only drawn when the series changes.

The same curated series selection as the other comparison scripts is applied
when those series are available in the paired range.

The current C++ export sequence has no counterparts for `48_maku` and
`49_maku`, so those ground-truth frames are skipped before pairing.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

from PIL import Image, ImageDraw

from make_portage_bands import (
    CURATED_SERIES_SELECTIONS,
    DEFAULT_FIGURES_DIR,
    DEFAULT_OUTPUT_DIR,
    GT_PREFIX,
    draw_outlined_text,
    find_font,
    fit_font,
    load_and_scale,
    split_key,
    text_bbox,
)


DEFAULT_OUTPUT = DEFAULT_OUTPUT_DIR / "ground-truth-vs-cpp-port-bands.png"
CPP_PREFIX = "cpp_img_"
CPP_MISSING_GT_KEYS = (
    "48_maku",
    "49_maku",
)

Image.MAX_IMAGE_PIXELS = None


@dataclass(frozen=True)
class ImagePair:
    key: str
    index: int
    series: str
    gt_path: Path
    cpp_path: Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Build a two-band comparison image from gt/cpp PNGs."
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
        "--gt-start-key",
        default="11_mute95",
        help=(
            "Ground-truth suffix to start from, e.g. '11_mute95' or "
            "'gt_img_11_mute95.png' (default: 11_mute95)"
        ),
    )
    parser.add_argument(
        "--cpp-start-file",
        default="cpp_img_001364.png",
        help=(
            "First C++ port file in alphanumeric order, e.g. "
            "'cpp_img_001364.png' (default: cpp_img_001364.png)"
        ),
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=None,
        help="Optional number of paired images to keep from the starting point.",
    )
    parser.add_argument(
        "--tile-scale",
        type=float,
        default=1.0,
        help="Scale factor applied to every source image (default: 1.0).",
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
        "--single-strip",
        action="store_true",
        help="Keep the legacy two-row horizontal strip instead of stacking two half-strips.",
    )
    return parser.parse_args()


def normalize_gt_start_key(value: str) -> str:
    name = Path(value).name
    if name.endswith(".png"):
        name = name[:-4]
    if name.startswith(GT_PREFIX):
        return name[len(GT_PREFIX) :]
    return name


def normalize_cpp_start_name(value: str) -> str:
    name = Path(value).name
    if not name.endswith(".png"):
        name = f"{name}.png"
    if not name.startswith(CPP_PREFIX):
        name = f"{CPP_PREFIX}{name}"
    return name


def extract_gt_key(path: Path) -> str:
    stem = path.stem
    if not stem.startswith(GT_PREFIX):
        raise ValueError(f"Unexpected ground-truth filename: {path.name}")
    return stem[len(GT_PREFIX) :]


def split_pairs_for_stacked_layout(pairs: list[ImagePair]) -> list[list[ImagePair]]:
    if len(pairs) <= 1:
        return [pairs]
    split_index = (len(pairs) + 1) // 2
    return [pairs[:split_index], pairs[split_index:]]


def select_curated_available_pairs(pairs: list[ImagePair]) -> list[ImagePair]:
    filtered_pairs = []
    series_positions: dict[str, int] = {}

    for pair in pairs:
        position = series_positions.get(pair.series, 0) + 1
        series_positions[pair.series] = position
        allowed_positions = CURATED_SERIES_SELECTIONS.get(pair.series)
        if allowed_positions is None or position in allowed_positions:
            filtered_pairs.append(pair)

    for series, available in series_positions.items():
        allowed_positions = CURATED_SERIES_SELECTIONS.get(series)
        if allowed_positions is None:
            continue
        missing = [position for position in allowed_positions if position > available]
        if missing:
            raise SystemExit(
                f"Series {series!r} does not have requested image position(s): {missing}"
            )

    return filtered_pairs


def collect_pairs(
    figures_dir: Path,
    gt_start_key: str,
    cpp_start_name: str,
    limit: int | None,
) -> tuple[list[ImagePair], int, int]:
    gt_files = sorted(
        figures_dir.glob(f"{GT_PREFIX}*.png"),
        key=lambda path: split_key(extract_gt_key(path)),
    )
    cpp_files = sorted(figures_dir.glob(f"{CPP_PREFIX}*.png"))

    if not gt_files:
        raise SystemExit(f"No ground-truth images found in {figures_dir}")
    if not cpp_files:
        raise SystemExit(f"No C++ port images found in {figures_dir}")

    gt_keys = [extract_gt_key(path) for path in gt_files]
    try:
        gt_start_index = gt_keys.index(gt_start_key)
    except ValueError as exc:
        preview = ", ".join(gt_keys[:8])
        raise SystemExit(
            f"Ground-truth start key {gt_start_key!r} not found. First keys: {preview}"
        ) from exc

    cpp_names = [path.name for path in cpp_files]
    try:
        cpp_start_index = cpp_names.index(cpp_start_name)
    except ValueError as exc:
        preview = ", ".join(cpp_names[:8])
        raise SystemExit(
            f"C++ start file {cpp_start_name!r} not found. First files: {preview}"
        ) from exc

    gt_selected = gt_files[gt_start_index:]
    cpp_selected = cpp_files[cpp_start_index:]
    gt_selected_for_cpp = [
        path for path in gt_selected if extract_gt_key(path) not in CPP_MISSING_GT_KEYS
    ]
    skipped_gt_count = len(gt_selected) - len(gt_selected_for_cpp)

    pair_count = min(len(gt_selected_for_cpp), len(cpp_selected))
    if limit is not None:
        if limit <= 0:
            raise SystemExit("--limit must be a positive integer")
        pair_count = min(pair_count, limit)
    if pair_count == 0:
        raise SystemExit("No overlapping images remain after applying the start positions")

    pairs = []
    for gt_path, cpp_path in zip(gt_selected_for_cpp[:pair_count], cpp_selected[:pair_count]):
        key = extract_gt_key(gt_path)
        index, series = split_key(key)
        pairs.append(
            ImagePair(
                key=key,
                index=index,
                series=series,
                gt_path=gt_path,
                cpp_path=cpp_path,
            )
        )

    pairs = select_curated_available_pairs(pairs)
    if not pairs:
        raise SystemExit("No curated gt/cpp pairs remain after applying the filters")

    unused_gt = skipped_gt_count + max(0, len(gt_selected_for_cpp) - pair_count)
    unused_cpp = max(0, len(cpp_selected) - pair_count)
    return pairs, unused_gt, unused_cpp


def build_composite(
    pairs: list[ImagePair],
    output_path: Path,
    regular_font_path: Path,
    title_font_path: Path,
    background: str,
    tile_scale: float,
    single_strip: bool,
) -> None:
    first = Image.open(pairs[0].gt_path)
    base_width, base_height = first.size
    tile_width = max(1, round(base_width * tile_scale))
    tile_height = max(1, round(base_height * tile_scale))
    tile_size = (tile_width, tile_height)

    outer_padding = max(20, round(tile_height * 0.08))
    tile_gap = max(6, round(tile_width * 0.015))
    band_gap = max(20, round(tile_height * 0.08))
    title_gutter = max(180, round(tile_width * 0.36))
    label_margin = max(10, round(tile_height * 0.05))

    label_stroke = max(2, round(tile_height * 0.012))
    title_stroke = max(2, round(tile_height * 0.01))

    title_font = fit_font(
        ["reference", "C++\nport"],
        title_font_path,
        max_size=max(26, round(tile_height * 0.24)),
        min_size=18,
        max_width=title_gutter - 2 * label_margin,
        max_height=tile_height - 2 * label_margin,
        stroke_width=title_stroke,
    )
    label_font = fit_font(
        sorted({pair.series for pair in pairs}, key=len, reverse=True)[:6] or ["mute95"],
        regular_font_path,
        max_size=max(18, round(tile_height * 0.15)),
        min_size=14,
        max_width=tile_width - 2 * label_margin,
        max_height=round(tile_height * 0.25),
        stroke_width=label_stroke,
    )

    pair_chunks = [pairs] if single_strip else split_pairs_for_stacked_layout(pairs)
    max_columns = max(len(chunk) for chunk in pair_chunks)
    block_gap = band_gap

    canvas_width = (
        outer_padding * 2
        + title_gutter
        + max_columns * tile_width
        + max(0, max_columns - 1) * tile_gap
    )
    canvas_height = (
        outer_padding * 2
        + len(pair_chunks) * (tile_height * 2 + band_gap)
        + max(0, len(pair_chunks) - 1) * block_gap
    )

    canvas = Image.new("RGBA", (canvas_width, canvas_height), background)
    draw = ImageDraw.Draw(canvas)

    row_x = outer_padding + title_gutter
    for chunk_index, chunk in enumerate(pair_chunks):
        block_top_y = outer_padding + chunk_index * (tile_height * 2 + band_gap + block_gap)
        top_y = block_top_y
        bottom_y = block_top_y + tile_height + band_gap

        for title, row_y in (("reference", top_y), ("C++\nport", bottom_y)):
            text_width, text_height = text_bbox(
                title_font, title, stroke_width=title_stroke
            )
            title_x = outer_padding + label_margin
            title_y = row_y + (tile_height - text_height) // 2
            if text_width > title_gutter - 2 * label_margin:
                title_x = outer_padding
            draw_outlined_text(draw, (title_x, title_y), title, title_font, title_stroke)

        previous_series: str | None = None
        for column, pair in enumerate(chunk):
            x = row_x + column * (tile_width + tile_gap)

            top_image = load_and_scale(pair.gt_path, tile_size)
            bottom_image = load_and_scale(pair.cpp_path, tile_size)
            canvas.alpha_composite(top_image, (x, top_y))
            canvas.alpha_composite(bottom_image, (x, bottom_y))

            if pair.series != previous_series:
                text_width, text_height = text_bbox(
                    label_font, pair.series, stroke_width=label_stroke
                )
                text_x = x + label_margin
                if text_width > tile_width - 2 * label_margin:
                    text_x = x + max(0, (tile_width - text_width) // 2)
                text_y_top = top_y + tile_height - text_height - label_margin
                text_y_bottom = bottom_y + tile_height - text_height - label_margin
                draw_outlined_text(
                    draw,
                    (text_x, text_y_top),
                    pair.series,
                    label_font,
                    label_stroke,
                )
                draw_outlined_text(
                    draw,
                    (text_x, text_y_bottom),
                    pair.series,
                    label_font,
                    label_stroke,
                )
            previous_series = pair.series

    output_path.parent.mkdir(parents=True, exist_ok=True)
    canvas.save(output_path)


def main() -> None:
    args = parse_args()
    if args.tile_scale <= 0:
        raise SystemExit("--tile-scale must be greater than 0")

    figures_dir = args.figures_dir.expanduser().resolve()
    output_path = args.output.expanduser().resolve()
    gt_start_key = normalize_gt_start_key(args.gt_start_key)
    cpp_start_name = normalize_cpp_start_name(args.cpp_start_file)

    if not figures_dir.exists():
        raise SystemExit(f"Figures directory not found: {figures_dir}")

    pairs, unused_gt, unused_cpp = collect_pairs(
        figures_dir=figures_dir,
        gt_start_key=gt_start_key,
        cpp_start_name=cpp_start_name,
        limit=args.limit,
    )
    regular_font_path = find_font(args.font, ("Roboto-Regular.ttf", "Roboto-Regular.otf"))
    title_font_path = find_font(
        None if args.font is None else args.font.with_name(args.font.name),
        ("Roboto-Bold.ttf", "Roboto-Bold.otf", "Roboto-Regular.ttf", "Roboto-Regular.otf"),
    )
    build_composite(
        pairs=pairs,
        output_path=output_path,
        regular_font_path=regular_font_path,
        title_font_path=title_font_path,
        background=args.background,
        tile_scale=args.tile_scale,
        single_strip=args.single_strip,
    )

    message = f"Generated {output_path} with {len(pairs)} paired images."
    if unused_gt or unused_cpp:
        remainders = []
        if unused_gt:
            remainders.append(f"{unused_gt} unmatched ground-truth image(s)")
        if unused_cpp:
            remainders.append(f"{unused_cpp} unmatched C++ image(s)")
        message = f"{message} Left aside: {', '.join(remainders)}."
    print(message)


if __name__ == "__main__":
    main()
