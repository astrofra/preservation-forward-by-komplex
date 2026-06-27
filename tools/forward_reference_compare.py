#!/usr/bin/env python3
from __future__ import annotations

import argparse
import csv
import math
import statistics
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


SCENE_TARGETS: dict[str, list[str]] = {
    "mute95": ["java-desktop/src/main/java/kmjjkmk.java"],
    "domina": ["java-desktop/src/main/java/kajakka.java"],
    "saari": ["java-desktop/src/main/java/maajmka.java"],
    "kukot": ["java-desktop/src/main/java/kajjkka.java"],
    "maku": ["java-desktop/src/main/java/kmjjmka.java"],
    "watercube": ["java-desktop/src/main/java/kmajmka.java"],
    "feta": ["java-desktop/src/main/java/kmaamka.java"],
    "uppol": ["java-desktop/src/main/java/mmaakmk.java"],
}

GLOBAL_TARGETS = [
    "java-desktop/src/main/java/forward.java",
    "java-desktop/src/main/java/kaaakka.java",
    "java-desktop/src/main/java/mmaamma.java",
    "java-desktop/src/main/java/kaajmma.java",
]


@dataclass
class CaptureRow:
    capture_index: int
    render_frame: int
    demo_time_ms: int
    demo_time_seconds: float
    scene: str
    next_script_time_hex: str
    frame_path: Path


@dataclass
class ReferenceRow:
    capture_index: int
    demo_time_ms: int
    video_time_ms: int
    scene: str
    frame_path: Path


@dataclass
class FrameMetrics:
    capture_index: int
    scene: str
    demo_time_ms: int
    render_frame: int
    mae: float
    rmse: float
    psnr: float
    changed_ratio: float
    mean_luma_delta: float
    max_channel_diff: int
    diff_scope: str
    bbox: str
    java_frame_path: Path
    reference_frame_path: Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Extract reference frames from the YouTube capture and compare them with the Java self-captures."
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    extract = subparsers.add_parser("extract-video", help="Extract reference frames from the video using the Java manifest timestamps.")
    extract.add_argument("--video", required=True, type=Path)
    extract.add_argument("--java-manifest", required=True, type=Path)
    extract.add_argument("--output-dir", required=True, type=Path)
    extract.add_argument("--video-offset-ms", type=int, default=0, help="Shift applied to Java timestamps before seeking in the reference video.")
    extract.add_argument(
        "--video-filter",
        default="scale=512:256:flags=lanczos",
        help="FFmpeg video filter applied before writing each extracted frame.",
    )
    extract.add_argument("--overwrite", action="store_true")

    compare = subparsers.add_parser("compare", help="Compare Java captures and reference captures.")
    compare.add_argument("--java-manifest", required=True, type=Path)
    compare.add_argument("--reference-manifest", required=True, type=Path)
    compare.add_argument("--output-dir", required=True, type=Path)
    compare.add_argument("--diff-threshold", type=int, default=24, help="Per-pixel max channel delta used for changed_ratio and diff bbox.")
    compare.add_argument("--visual-limit", type=int, default=24, help="Generate compare/diff PNGs for the N worst frames.")
    compare.add_argument("--write-visuals", action="store_true")

    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if args.command == "extract-video":
        extract_video(args)
        return 0
    if args.command == "compare":
        compare_manifests(args)
        return 0
    raise ValueError(f"Unsupported command: {args.command}")


def extract_video(args: argparse.Namespace) -> None:
    java_rows = load_java_manifest(args.java_manifest)
    frames_dir = args.output_dir / "frames"
    frames_dir.mkdir(parents=True, exist_ok=True)
    reference_manifest = args.output_dir / "reference_manifest.csv"
    with reference_manifest.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["capture_index", "demo_time_ms", "video_time_ms", "scene", "frame_path"])
        for row in java_rows:
            video_time_ms = row.demo_time_ms + args.video_offset_ms
            if video_time_ms < 0:
                raise ValueError(
                    f"Negative seek time for capture {row.capture_index}. Increase --video-offset-ms."
                )
            output_name = f"ref_{row.capture_index:06d}_t{video_time_ms:08d}.png"
            output_path = frames_dir / output_name
            if output_path.exists() and not args.overwrite:
                writer.writerow(
                    [row.capture_index, row.demo_time_ms, video_time_ms, row.scene, relative_to(args.output_dir, output_path)]
                )
                continue
            run_ffmpeg(
                [
                    "ffmpeg",
                    "-v",
                    "error",
                    "-y",
                    "-ss",
                    format_seconds(video_time_ms),
                    "-i",
                    str(args.video),
                    "-frames:v",
                    "1",
                    "-vf",
                    args.video_filter,
                    str(output_path),
                ]
            )
            writer.writerow(
                [row.capture_index, row.demo_time_ms, video_time_ms, row.scene, relative_to(args.output_dir, output_path)]
            )
    print(f"Reference manifest written to {reference_manifest}")


def compare_manifests(args: argparse.Namespace) -> None:
    java_rows = {row.capture_index: row for row in load_java_manifest(args.java_manifest)}
    reference_rows = {row.capture_index: row for row in load_reference_manifest(args.reference_manifest)}
    shared_indexes = sorted(set(java_rows) & set(reference_rows))
    if not shared_indexes:
        raise ValueError("No shared capture_index values between the Java and reference manifests.")

    args.output_dir.mkdir(parents=True, exist_ok=True)
    visuals_dir = args.output_dir / "visuals"
    if args.write_visuals:
        visuals_dir.mkdir(parents=True, exist_ok=True)

    metrics_rows: list[FrameMetrics] = []
    for capture_index in shared_indexes:
        java_row = java_rows[capture_index]
        reference_row = reference_rows[capture_index]
        width_a, height_a, rgb_a = decode_image_rgb24(java_row.frame_path)
        width_b, height_b, rgb_b = decode_image_rgb24(reference_row.frame_path)
        if width_a != width_b or height_a != height_b:
            raise ValueError(
                f"Dimension mismatch for capture {capture_index}: Java {width_a}x{height_a}, reference {width_b}x{height_b}"
            )
        metrics_rows.append(
            compute_metrics(
                java_row=java_row,
                reference_row=reference_row,
                width=width_a,
                height=height_a,
                rgb_a=rgb_a,
                rgb_b=rgb_b,
                diff_threshold=args.diff_threshold,
            )
        )

    write_frame_report(args.output_dir / "frame_metrics.csv", metrics_rows)
    write_scene_summary(args.output_dir / "summary.md", metrics_rows)

    if args.write_visuals and args.visual_limit > 0:
        write_visual_diffs(visuals_dir, metrics_rows, args.visual_limit)

    print(f"Compared {len(metrics_rows)} frames.")
    print(f"Frame report: {args.output_dir / 'frame_metrics.csv'}")
    print(f"Scene summary: {args.output_dir / 'summary.md'}")


def load_java_manifest(path: Path) -> list[CaptureRow]:
    path = path.resolve()
    rows: list[CaptureRow] = []
    with path.open("r", newline="", encoding="utf-8-sig") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            rows.append(
                CaptureRow(
                    capture_index=int(row["capture_index"]),
                    render_frame=int(row["render_frame"]),
                    demo_time_ms=int(row["demo_time_ms"]),
                    demo_time_seconds=float(row["demo_time_seconds"]),
                    scene=row.get("scene", "") or "",
                    next_script_time_hex=row.get("next_script_time_hex", "") or "",
                    frame_path=resolve_manifest_path(path, row["frame_path"]),
                )
            )
    return rows


def load_reference_manifest(path: Path) -> list[ReferenceRow]:
    path = path.resolve()
    rows: list[ReferenceRow] = []
    with path.open("r", newline="", encoding="utf-8-sig") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            rows.append(
                ReferenceRow(
                    capture_index=int(row["capture_index"]),
                    demo_time_ms=int(row["demo_time_ms"]),
                    video_time_ms=int(row["video_time_ms"]),
                    scene=row.get("scene", "") or "",
                    frame_path=resolve_manifest_path(path, row["frame_path"]),
                )
            )
    return rows


def resolve_manifest_path(manifest_path: Path, frame_path: str) -> Path:
    path = Path(frame_path)
    if path.is_absolute():
        return path
    return (manifest_path.parent / path).resolve()


def relative_to(base_dir: Path, target_path: Path) -> str:
    return target_path.resolve().relative_to(base_dir.resolve()).as_posix()


def format_seconds(milliseconds: int) -> str:
    return f"{milliseconds / 1000.0:.3f}"


def decode_image_rgb24(path: Path) -> tuple[int, int, bytes]:
    width, height = probe_image_size(path)
    result = subprocess.run(
        [
            "ffmpeg",
            "-v",
            "error",
            "-i",
            str(path),
            "-f",
            "rawvideo",
            "-pix_fmt",
            "rgb24",
            "-",
        ],
        capture_output=True,
        check=True,
    )
    expected = width * height * 3
    if len(result.stdout) != expected:
        raise ValueError(f"Unexpected decoded size for {path}: expected {expected} bytes, got {len(result.stdout)}")
    return width, height, result.stdout


def probe_image_size(path: Path) -> tuple[int, int]:
    result = subprocess.run(
        [
            "ffprobe",
            "-v",
            "error",
            "-select_streams",
            "v:0",
            "-show_entries",
            "stream=width,height",
            "-of",
            "csv=s=x:p=0",
            str(path),
        ],
        capture_output=True,
        text=True,
        check=True,
    )
    width_text, height_text = result.stdout.strip().split("x", 1)
    return int(width_text), int(height_text)


def compute_metrics(
    *,
    java_row: CaptureRow,
    reference_row: ReferenceRow,
    width: int,
    height: int,
    rgb_a: bytes,
    rgb_b: bytes,
    diff_threshold: int,
) -> FrameMetrics:
    total_abs = 0
    total_sq = 0
    total_luma_delta = 0.0
    max_channel_diff = 0
    changed_pixels = 0
    min_x = width
    min_y = height
    max_x = -1
    max_y = -1

    pixel_count = width * height
    for pixel_index in range(pixel_count):
        base = pixel_index * 3
        dr = abs(rgb_a[base] - rgb_b[base])
        dg = abs(rgb_a[base + 1] - rgb_b[base + 1])
        db = abs(rgb_a[base + 2] - rgb_b[base + 2])
        total_abs += dr + dg + db
        total_sq += dr * dr + dg * dg + db * db
        pixel_max = max(dr, dg, db)
        if pixel_max > max_channel_diff:
            max_channel_diff = pixel_max
        luma_a = 0.2126 * rgb_a[base] + 0.7152 * rgb_a[base + 1] + 0.0722 * rgb_a[base + 2]
        luma_b = 0.2126 * rgb_b[base] + 0.7152 * rgb_b[base + 1] + 0.0722 * rgb_b[base + 2]
        total_luma_delta += luma_a - luma_b
        if pixel_max >= diff_threshold:
            changed_pixels += 1
            x = pixel_index % width
            y = pixel_index // width
            if x < min_x:
                min_x = x
            if y < min_y:
                min_y = y
            if x > max_x:
                max_x = x
            if y > max_y:
                max_y = y

    sample_count = pixel_count * 3
    mae = total_abs / sample_count
    rmse = math.sqrt(total_sq / sample_count)
    psnr = math.inf if rmse == 0 else 20.0 * math.log10(255.0 / rmse)
    changed_ratio = changed_pixels / pixel_count
    mean_luma_delta = total_luma_delta / pixel_count
    bbox = "" if changed_pixels == 0 else f"{min_x},{min_y},{max_x},{max_y}"
    diff_scope = classify_diff_scope(width, height, min_x, min_y, max_x, max_y, changed_pixels)

    return FrameMetrics(
        capture_index=java_row.capture_index,
        scene=java_row.scene or reference_row.scene,
        demo_time_ms=java_row.demo_time_ms,
        render_frame=java_row.render_frame,
        mae=mae,
        rmse=rmse,
        psnr=psnr,
        changed_ratio=changed_ratio,
        mean_luma_delta=mean_luma_delta,
        max_channel_diff=max_channel_diff,
        diff_scope=diff_scope,
        bbox=bbox,
        java_frame_path=java_row.frame_path,
        reference_frame_path=reference_row.frame_path,
    )


def classify_diff_scope(width: int, height: int, min_x: int, min_y: int, max_x: int, max_y: int, changed_pixels: int) -> str:
    if changed_pixels == 0:
        return "none"
    bbox_width = max_x - min_x + 1
    bbox_height = max_y - min_y + 1
    width_ratio = bbox_width / width
    height_ratio = bbox_height / height
    area_ratio = (bbox_width * bbox_height) / (width * height)
    if width_ratio >= 0.85 and height_ratio >= 0.85:
        return "global"
    if width_ratio >= 0.85 and height_ratio <= 0.5:
        return "horizontal-band"
    if height_ratio >= 0.85 and width_ratio <= 0.5:
        return "vertical-band"
    if area_ratio <= 0.25:
        return "local"
    return "mixed"


def write_frame_report(path: Path, metrics_rows: list[FrameMetrics]) -> None:
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(
            [
                "capture_index",
                "scene",
                "demo_time_ms",
                "render_frame",
                "mae",
                "rmse",
                "psnr",
                "changed_ratio",
                "mean_luma_delta",
                "max_channel_diff",
                "diff_scope",
                "bbox",
                "java_frame_path",
                "reference_frame_path",
            ]
        )
        for row in metrics_rows:
            writer.writerow(
                [
                    row.capture_index,
                    row.scene,
                    row.demo_time_ms,
                    row.render_frame,
                    f"{row.mae:.4f}",
                    f"{row.rmse:.4f}",
                    "inf" if math.isinf(row.psnr) else f"{row.psnr:.4f}",
                    f"{row.changed_ratio:.4f}",
                    f"{row.mean_luma_delta:.4f}",
                    row.max_channel_diff,
                    row.diff_scope,
                    row.bbox,
                    row.java_frame_path.as_posix(),
                    row.reference_frame_path.as_posix(),
                ]
            )


def write_scene_summary(path: Path, metrics_rows: list[FrameMetrics]) -> None:
    by_scene: dict[str, list[FrameMetrics]] = {}
    for row in metrics_rows:
        by_scene.setdefault(row.scene or "unknown", []).append(row)

    global_mae = statistics.mean(row.mae for row in metrics_rows)
    global_changed = statistics.mean(row.changed_ratio for row in metrics_rows)

    lines: list[str] = []
    lines.append("# Forward Capture Comparison")
    lines.append("")
    lines.append(f"- Frames compared: {len(metrics_rows)}")
    lines.append(f"- Global mean absolute error: {global_mae:.2f}")
    lines.append(f"- Global changed ratio: {global_changed:.2%}")
    lines.append("")

    if global_changed >= 0.70:
        lines.append("## Global Check")
        lines.append("")
        lines.append("- Large full-frame mismatch across the run. Verify `--video-offset-ms` and the reference crop/scale filter before changing Java code.")
        lines.append(f"- If the alignment is already correct, inspect: {', '.join(GLOBAL_TARGETS)}")
        lines.append("")

    lines.append("## Scene Priorities")
    lines.append("")
    lines.append("| Scene | Frames | Avg MAE | Avg Changed | Worst Capture | Suggested Focus |")
    lines.append("|---|---:|---:|---:|---:|---|")

    ranked_scenes = sorted(
        by_scene.items(),
        key=lambda item: statistics.mean(row.mae + row.changed_ratio * 64.0 for row in item[1]),
        reverse=True,
    )
    for scene, scene_rows in ranked_scenes:
        avg_mae = statistics.mean(row.mae for row in scene_rows)
        avg_changed = statistics.mean(row.changed_ratio for row in scene_rows)
        worst_row = max(scene_rows, key=lambda row: row.mae + row.changed_ratio * 64.0)
        lines.append(
            f"| {scene} | {len(scene_rows)} | {avg_mae:.2f} | {avg_changed:.2%} | {worst_row.capture_index} | {heuristic_focus(scene, scene_rows)} |"
        )

    lines.append("")
    lines.append("## Suggested Actions")
    lines.append("")
    for scene, scene_rows in ranked_scenes:
        lines.extend(build_scene_actions(scene, scene_rows))
        lines.append("")

    path.write_text("\n".join(lines).rstrip() + "\n", encoding="utf-8")


def heuristic_focus(scene: str, rows: list[FrameMetrics]) -> str:
    avg_changed = statistics.mean(row.changed_ratio for row in rows)
    avg_luma = statistics.mean(abs(row.mean_luma_delta) for row in rows)
    scopes = {row.diff_scope for row in rows}
    if avg_changed >= 0.70:
        return "timing / scene sequencing"
    if avg_luma >= 18.0 and "global" in scopes:
        return "palette / fade / post effect"
    if "horizontal-band" in scopes or "vertical-band" in scopes:
        return "scroll / wrap / viewport"
    if avg_changed <= 0.25:
        return "camera / raster / local effect"
    return "scene-specific render path"


def build_scene_actions(scene: str, rows: list[FrameMetrics]) -> list[str]:
    avg_mae = statistics.mean(row.mae for row in rows)
    avg_changed = statistics.mean(row.changed_ratio for row in rows)
    avg_luma = statistics.mean(abs(row.mean_luma_delta) for row in rows)
    scopes = {row.diff_scope for row in rows}
    targets = SCENE_TARGETS.get(scene, GLOBAL_TARGETS)
    lines = [f"### {scene}"]

    if avg_changed >= 0.70:
        lines.append(
            f"- Diff is mostly structural (`changed_ratio` {avg_changed:.2%}). Check scene activation timing, message handling, and time-driven state in `{targets[0]}`."
        )
    elif avg_luma >= 18.0 and "global" in scopes:
        lines.append(
            f"- Brightness or fade drift dominates (`|mean_luma_delta|` {avg_luma:.1f}). Inspect blend/fade/palette logic in `{targets[0]}` and the shared framebuffer code."
        )
    elif "horizontal-band" in scopes or "vertical-band" in scopes:
        lines.append(
            f"- Mismatch is concentrated in a large strip. Prioritize scroll, wrap, and copy-window logic in `{targets[0]}`."
        )
    elif avg_changed <= 0.25 and avg_mae >= 8.0:
        lines.append(
            f"- Geometry mostly lines up but fine detail does not. Inspect texture mapping, projection, or local feedback effects in `{targets[0]}`."
        )
    else:
        lines.append(
            f"- Mixed mismatch pattern. Start with `{targets[0]}` and compare against the worst capture of the scene."
        )

    if len(targets) > 1:
        lines.append(f"- Secondary files: {', '.join(targets[1:])}")
    lines.append(f"- Scene average: MAE {avg_mae:.2f}, changed ratio {avg_changed:.2%}.")
    return lines


def write_visual_diffs(visuals_dir: Path, metrics_rows: list[FrameMetrics], visual_limit: int) -> None:
    ranked_rows = sorted(metrics_rows, key=lambda row: row.mae + row.changed_ratio * 64.0, reverse=True)[:visual_limit]
    for row in ranked_rows:
        prefix = visuals_dir / f"{row.capture_index:06d}_{row.scene or 'unknown'}"
        compare_path = prefix.with_name(prefix.name + "_compare.png")
        diff_path = prefix.with_name(prefix.name + "_diff.png")
        run_ffmpeg(
            [
                "ffmpeg",
                "-v",
                "error",
                "-y",
                "-i",
                str(row.java_frame_path),
                "-i",
                str(row.reference_frame_path),
                "-filter_complex",
                "[0:v][1:v]hstack=inputs=2[v]",
                "-map",
                "[v]",
                str(compare_path),
            ]
        )
        run_ffmpeg(
            [
                "ffmpeg",
                "-v",
                "error",
                "-y",
                "-i",
                str(row.java_frame_path),
                "-i",
                str(row.reference_frame_path),
                "-filter_complex",
                "[0:v][1:v]blend=all_mode=difference,eq=contrast=2.0:brightness=0.02[v]",
                "-map",
                "[v]",
                str(diff_path),
            ]
        )


def run_ffmpeg(command: list[str]) -> None:
    completed = subprocess.run(command, capture_output=True, text=True)
    if completed.returncode != 0:
        sys.stderr.write(completed.stderr)
        raise subprocess.CalledProcessError(completed.returncode, command)


if __name__ == "__main__":
    raise SystemExit(main())
