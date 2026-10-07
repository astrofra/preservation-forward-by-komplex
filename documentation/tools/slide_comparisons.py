"""Shared three-pair selection for the three slide comparison scripts.

The selection is visual/editorial, based on the naive-port comparison:
Saari loses its island, Kukot loses its metallic appearance, Maku becomes
white.
Keep the same order across all three outputs. C++ uses the nearby Maku
reference 50 because reference 48 has no corresponding C++ capture.
"""

from __future__ import annotations

import argparse
from pathlib import Path

import make_cpp_port_bands as cpp
import make_java_reconstruction_bands as java
import make_portage_bands as naive
from PIL import Image, ImageDraw, ImageOps


SELECTED_KEYS = (
    "28_saari",
    "39_kukot",
    "48_maku",
)
CPP_REFERENCE_SUBSTITUTIONS = {"48_maku": "50_maku"}
OUTPUT_NAMES = {
    "naive": "reference-vs-naive-port-bands-slides.png",
    "java": "reference-vs-java-reconstruction-bands-slides.png",
    "cpp": "reference-vs-cpp-port-bands-slides.png",
}
PORT_LABELS = {
    "naive": "C++ naive port",
    "java": "Java reconstruction",
    "cpp": "C++ port",
}


def build_slide(pairs, variant, output, regular_font, bold_font, background, scale):
    """Fill a 3 x 2 grid, preserving the original 2816:1124 canvas ratio.

    Center-crop each capture to fill its cell without distortion. Both images
    in a comparison use the same crop; only thin gutters remain uncovered.
    """
    width, height = 2816 * scale, 1124 * scale
    gap = 8 * scale
    columns = len(pairs)
    image_width = width - (columns - 1) * gap
    tile_height = (height - gap) // 2
    label_margin, label_stroke = 24 * scale, 5 * scale
    canvas = Image.new("RGBA", (width, height), background)
    draw = ImageDraw.Draw(canvas)
    scene_font = naive.fit_font(
        [pair.series for pair in pairs], regular_font,
        max_size=68 * scale, min_size=24 * scale,
        max_width=image_width // columns - 2 * label_margin, max_height=80 * scale,
        stroke_width=label_stroke,
    )
    first_scene_width, _ = naive.text_bbox(scene_font, pairs[0].series, label_stroke)
    heading_font = naive.fit_font(
        ["reference", PORT_LABELS[variant]], bold_font,
        max_size=68 * scale, min_size=24 * scale,
        max_width=image_width // columns - 3 * label_margin - first_scene_width,
        max_height=90 * scale,
        stroke_width=label_stroke,
    )
    comparison_field = {"naive": "naive_path", "java": "java_path", "cpp": "cpp_path"}[variant]
    for row, (label, field) in enumerate([
        ("reference", "gt_path"), (PORT_LABELS[variant], comparison_field),
    ]):
        for column, pair in enumerate(pairs):
            x = column * gap + column * image_width // columns
            tile_width = (column + 1) * image_width // columns - column * image_width // columns
            y = row * (tile_height + gap)
            with Image.open(getattr(pair, field)) as source:
                tile = ImageOps.fit(
                    source.convert("RGBA"), (tile_width, tile_height),
                    method=Image.Resampling.LANCZOS, centering=(0.5, 0.5),
                )
            canvas.alpha_composite(tile, (x, y))
            if column == 0:
                draw.text(
                    (x + label_margin, y + tile_height - label_margin), label,
                    font=heading_font, fill="white", anchor="lb",
                    stroke_width=label_stroke, stroke_fill="black",
                )
            draw.text(
                (x + tile_width - label_margin, y + tile_height - label_margin), pair.series,
                font=scene_font, fill="white", anchor="rb",
                stroke_width=label_stroke, stroke_fill="black",
            )

    output.parent.mkdir(parents=True, exist_ok=True)
    canvas.save(output)


def main(variant: str) -> None:
    parser = argparse.ArgumentParser(
        description="Build a slide comparison with three selected pairs (6 images)."
    )
    parser.add_argument("--figures-dir", type=Path, default=naive.DEFAULT_FIGURES_DIR)
    parser.add_argument(
        "--output", type=Path,
        default=naive.DEFAULT_OUTPUT_DIR / OUTPUT_NAMES[variant],
    )
    parser.add_argument("--font", type=Path, help="Override the bundled Roboto font.")
    parser.add_argument("--background", default="#000000")
    parser.add_argument("--scale", type=int, default=1, help="Positive integer output scale; preserves the canvas ratio exactly.")
    args = parser.parse_args()
    if args.scale <= 0:
        parser.error("--scale must be greater than 0")

    figures_dir = args.figures_dir.expanduser().resolve()
    if variant == "naive":
        pairs = naive.collect_pairs(figures_dir, "11_mute95", None)
    elif variant == "java":
        pairs, _, _ = java.collect_pairs(figures_dir, "11_mute95", "java_img_007694.png", None)
    else:
        pairs, _, _ = cpp.collect_pairs(figures_dir, "11_mute95", "cpp_img_001364.png", None)

    by_key = {pair.key: pair for pair in pairs}
    keys = [
        CPP_REFERENCE_SUBSTITUTIONS.get(key, key) if variant == "cpp" else key
        for key in SELECTED_KEYS
    ]
    missing = [key for key in keys if key not in by_key]
    if missing:
        parser.error(f"Missing selected comparison pairs: {', '.join(missing)}")
    selected = [by_key[key] for key in keys]

    regular_font = naive.find_font(args.font, ("Roboto-Regular.ttf", "Roboto-Regular.otf"))
    title_font = naive.find_font(args.font, ("Roboto-Bold.ttf", "Roboto-Bold.otf"))
    output = args.output.expanduser().resolve()
    build_slide(
        selected, variant, output, regular_font, title_font,
        args.background, args.scale,
    )
    print(f"Generated {output} with {len(selected)} pairs ({2 * len(selected)} images): {', '.join(keys)}.")
    if variant == "cpp":
        print("Maku uses reference 50 instead of 48, as in the full C++ comparison.")
