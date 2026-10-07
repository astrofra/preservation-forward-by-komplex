"""Shared five-pair selection for the three slide comparison scripts.

The selection is visual/editorial, based on the naive-port comparison:
Saari loses its island, Kukot loses its metallic appearance, Maku becomes
white, Watercube loses the object's shading, and Feta changes orientation.
Keep the same order across all three outputs. C++ uses the nearby Maku
reference 50 because reference 48 has no corresponding C++ capture.
"""

from __future__ import annotations

import argparse
from pathlib import Path

import make_cpp_port_bands as cpp
import make_java_reconstruction_bands as java
import make_portage_bands as naive
from PIL import Image, ImageDraw


SELECTED_KEYS = (
    "28_saari",
    "39_kukot",
    "48_maku",
    "58_watercube",
    "63_feta",
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
    """Five aligned columns, preserving the original 2816:1124 canvas ratio."""
    width, height = 2816 * scale, 1124 * scale
    padding, gap = 40 * scale, 20 * scale
    # Even width preserves the captures' 2:1 aspect ratio exactly.
    tile_width = 2 * ((width - 2 * padding - 4 * gap) // 10)
    tile_height = tile_width // 2
    row_width = 5 * tile_width + 4 * gap
    left = (width - row_width) // 2
    scene_height, heading_height, block_gap = 70 * scale, 80 * scale, 64 * scale
    content_height = scene_height + 2 * (heading_height + tile_height) + block_gap
    top = (height - content_height) // 2
    canvas = Image.new("RGBA", (width, height), background)
    draw = ImageDraw.Draw(canvas)
    scene_font = naive.fit_font(
        [pair.series for pair in pairs], regular_font,
        max_size=52 * scale, min_size=24 * scale,
        max_width=tile_width, max_height=scene_height,
    )
    heading_font = naive.fit_font(
        ["reference", PORT_LABELS[variant]], bold_font,
        max_size=60 * scale, min_size=24 * scale,
        max_width=row_width, max_height=heading_height,
    )
    for column, pair in enumerate(pairs):
        x = left + column * (tile_width + gap)
        draw.text(
            (x + tile_width // 2, top), pair.series,
            font=scene_font, fill="white", anchor="mt",
        )

    comparison_field = {"naive": "naive_path", "java": "java_path", "cpp": "cpp_path"}[variant]
    for row, (label, field) in enumerate([
        ("reference", "gt_path"), (PORT_LABELS[variant], comparison_field),
    ]):
        heading_y = top + scene_height + row * (heading_height + tile_height + block_gap)
        draw.text((left, heading_y), label, font=heading_font, fill="white", anchor="lt")
        for column, pair in enumerate(pairs):
            tile = naive.load_and_scale(getattr(pair, field), (tile_width, tile_height))
            canvas.alpha_composite(tile, (left + column * (tile_width + gap), heading_y + heading_height))

    output.parent.mkdir(parents=True, exist_ok=True)
    canvas.save(output)


def main(variant: str) -> None:
    parser = argparse.ArgumentParser(
        description="Build a slide comparison with five selected pairs (10 images)."
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
    print(f"Generated {output} with 5 pairs (10 images): {', '.join(keys)}.")
    if variant == "cpp":
        print("Maku uses reference 50 instead of 48, as in the full C++ comparison.")
