#!/usr/bin/env python3
"""
Assemble two horizontal image bands from img/figures.

Default use:
    python3 make_portage_bands.py

This builds one composite PNG with:
- either a single two-row strip;
- or, by default, two stacked half-strips arranged as:
  GT / naive port / GT / naive port.

For each image, the series name (`mute95`, `saari`, etc.) is written in the
bottom-left corner in white Roboto with a black outline. When the same series
appears on consecutive images, the label is only drawn on the first one.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

try:
    from PIL import Image, ImageDraw, ImageFont
except ImportError as exc:  # pragma: no cover - runtime dependency guard
    raise SystemExit(
        "Pillow is required. Install it with: python3 -m pip install Pillow"
    ) from exc


SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_FIGURES_DIR = SCRIPT_DIR.parent.parent / "img" / "figures"
DEFAULT_OUTPUT_DIR = DEFAULT_FIGURES_DIR / "output"
DEFAULT_OUTPUT = DEFAULT_OUTPUT_DIR / "ground-truth-vs-naive-port-bands.png"
GT_PREFIX = "gt_img_"
NAIVE_PREFIX = "naive-port_img_"
CURATED_SERIES_SELECTIONS: dict[str, tuple[int, ...]] = {
    "saari": (1, 3),
    "kukot": (1, 2),
    "maku": (1,),
    "watercube": (2,),
    "feta": (1,),
    "uppol": (1,),
}

FONT_SEARCH_ROOTS = (
    SCRIPT_DIR / "fonts",
    Path.home() / "Library" / "Fonts",
    Path("/Library/Fonts"),
    Path("/System/Library/Fonts"),
)

Image.MAX_IMAGE_PIXELS = None


@dataclass(frozen=True)
class ImagePair:
    key: str
    index: int
    series: str
    gt_path: Path
    naive_path: Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Build a two-band comparison image from gt/naive-port PNGs."
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
        "--start-key",
        default="11_mute95",
        help=(
            "Start at this shared suffix, e.g. '11_mute95' or "
            "'gt_img_11_mute95.png' (default: 11_mute95)"
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


def normalize_start_key(value: str) -> str:
    name = Path(value).name
    if name.endswith(".png"):
        name = name[:-4]
    if name.startswith(GT_PREFIX):
        return name[len(GT_PREFIX) :]
    if name.startswith(NAIVE_PREFIX):
        return name[len(NAIVE_PREFIX) :]
    return name


def split_key(key: str) -> tuple[int, str]:
    index_text, series = key.split("_", 1)
    return int(index_text), series


def extract_key(path: Path, prefix: str) -> str:
    stem = path.stem
    if not stem.startswith(prefix):
        raise ValueError(f"Unexpected filename for prefix {prefix!r}: {path.name}")
    return stem[len(prefix) :]


def select_curated_pairs(pairs: list[ImagePair]) -> list[ImagePair]:
    filtered_pairs = []
    series_positions: dict[str, int] = {}

    for pair in pairs:
        position = series_positions.get(pair.series, 0) + 1
        series_positions[pair.series] = position
        allowed_positions = CURATED_SERIES_SELECTIONS.get(pair.series)
        if allowed_positions is None or position in allowed_positions:
            filtered_pairs.append(pair)

    for series, allowed_positions in CURATED_SERIES_SELECTIONS.items():
        available = series_positions.get(series, 0)
        missing = [position for position in allowed_positions if position > available]
        if missing:
            raise SystemExit(
                f"Series {series!r} does not have requested image position(s): {missing}"
            )

    return filtered_pairs


def collect_pairs(figures_dir: Path, start_key: str, limit: int | None) -> list[ImagePair]:
    gt_files = {
        extract_key(path, GT_PREFIX): path for path in figures_dir.glob(f"{GT_PREFIX}*.png")
    }
    naive_files = {
        extract_key(path, NAIVE_PREFIX): path
        for path in figures_dir.glob(f"{NAIVE_PREFIX}*.png")
    }
    common_keys = sorted(set(gt_files) & set(naive_files), key=split_key)
    if not common_keys:
        raise SystemExit(f"No shared gt/naive-port pairs found in {figures_dir}")

    try:
        start_index = common_keys.index(start_key)
    except ValueError as exc:
        available = ", ".join(common_keys[:8])
        raise SystemExit(
            f"Start key {start_key!r} not found. First available keys: {available}"
        ) from exc

    pairs = []
    for key in common_keys:
        index, series = split_key(key)
        pairs.append(
            ImagePair(
                key=key,
                index=index,
                series=series,
                gt_path=gt_files[key],
                naive_path=naive_files[key],
            )
        )

    curated_pairs = select_curated_pairs(pairs)
    key_order = {key: order for order, key in enumerate(common_keys)}
    selected_pairs = [
        pair for pair in curated_pairs if key_order[pair.key] >= start_index
    ]
    if limit is not None:
        if limit <= 0:
            raise SystemExit("--limit must be a positive integer")
        selected_pairs = selected_pairs[:limit]
    if not selected_pairs:
        raise SystemExit("No curated gt/naive-port pairs remain after applying the filters")
    return selected_pairs


def find_font(explicit_path: Path | None, preferred_names: tuple[str, ...]) -> Path:
    if explicit_path is not None:
        if explicit_path.exists():
            return explicit_path
        raise SystemExit(f"Font not found: {explicit_path}")

    for root in FONT_SEARCH_ROOTS:
        for name in preferred_names:
            candidate = root / name
            if candidate.exists():
                return candidate
    raise SystemExit(
        "Roboto font not found automatically. Pass it explicitly with --font."
    )


def text_bbox(font: ImageFont.FreeTypeFont, text: str, stroke_width: int = 0) -> tuple[int, int]:
    probe = Image.new("RGBA", (8, 8), (0, 0, 0, 0))
    draw = ImageDraw.Draw(probe)
    if "\n" in text:
        left, top, right, bottom = draw.multiline_textbbox(
            (0, 0),
            text,
            font=font,
            stroke_width=stroke_width,
            spacing=max(4, font.size // 6),
            align="left",
        )
    else:
        left, top, right, bottom = draw.textbbox(
            (0, 0),
            text,
            font=font,
            stroke_width=stroke_width,
        )
    return right - left, bottom - top


def fit_font(
    text_samples: list[str],
    font_path: Path,
    max_size: int,
    min_size: int,
    max_width: int,
    max_height: int,
    stroke_width: int = 0,
) -> ImageFont.FreeTypeFont:
    for size in range(max_size, min_size - 1, -1):
        font = ImageFont.truetype(str(font_path), size=size)
        if all(
            text_bbox(font, sample, stroke_width=stroke_width)[0] <= max_width
            and text_bbox(font, sample, stroke_width=stroke_width)[1] <= max_height
            for sample in text_samples
        ):
            return font
    return ImageFont.truetype(str(font_path), size=min_size)


def draw_outlined_text(
    draw: ImageDraw.ImageDraw,
    position: tuple[int, int],
    text: str,
    font: ImageFont.FreeTypeFont,
    stroke_width: int,
) -> None:
    if "\n" in text:
        draw.multiline_text(
            position,
            text,
            font=font,
            fill="white",
            stroke_width=stroke_width,
            stroke_fill="black",
            spacing=max(4, font.size // 6),
            align="left",
        )
        return
    draw.text(
        position,
        text,
        font=font,
        fill="white",
        stroke_width=stroke_width,
        stroke_fill="black",
    )


def load_and_scale(path: Path, tile_size: tuple[int, int]) -> Image.Image:
    image = Image.open(path).convert("RGBA")
    if image.size != tile_size:
        return image.resize(tile_size, Image.Resampling.LANCZOS)
    return image


def split_pairs_for_stacked_layout(pairs: list[ImagePair]) -> list[list[ImagePair]]:
    if len(pairs) <= 1:
        return [pairs]
    split_index = (len(pairs) + 1) // 2
    return [pairs[:split_index], pairs[split_index:]]


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
        ["reference", "C++\nnaive\nport"],
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

        title_texts = (("reference", top_y), ("C++\nnaive\nport", bottom_y))
        for title, row_y in title_texts:
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
            bottom_image = load_and_scale(pair.naive_path, tile_size)
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
    start_key = normalize_start_key(args.start_key)

    if not figures_dir.exists():
        raise SystemExit(f"Figures directory not found: {figures_dir}")

    pairs = collect_pairs(figures_dir, start_key=start_key, limit=args.limit)
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
    print(f"Generated {output_path} with {len(pairs)} paired images.")


if __name__ == "__main__":
    main()
