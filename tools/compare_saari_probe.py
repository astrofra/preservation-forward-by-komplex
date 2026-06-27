from __future__ import annotations

import argparse
import csv
import hashlib
import math
from pathlib import Path


PROJECTED_NUMERIC_FIELDS = [
    "camera_x",
    "camera_y",
    "depth_z",
    "screen_x_fp",
    "screen_y_fp",
    "screen_x_px",
    "screen_y_px",
    "uv_u",
    "uv_v",
]
PROJECTED_ENUM_FIELDS = ["clip_flags"]

TRIANGLE_NUMERIC_FIELDS = [
    "depth_sum",
    "x0_fp",
    "y0_fp",
    "z0",
    "x0_px",
    "y0_px",
    "u0",
    "v0",
    "x1_fp",
    "y1_fp",
    "z1",
    "x1_px",
    "y1_px",
    "u1",
    "v1",
    "x2_fp",
    "y2_fp",
    "z2",
    "x2_px",
    "y2_px",
    "u2",
    "v2",
]
TRIANGLE_ENUM_FIELDS = [
    "source_face_index",
    "synthetic",
    "material_id",
    "vertex0_index",
    "vertex1_index",
    "vertex2_index",
    "uv0_index",
    "uv1_index",
    "uv2_index",
    "flags0",
    "flags1",
    "flags2",
]


def main() -> int:
    parser = argparse.ArgumentParser(description="Compare two Forward saari probe outputs.")
    parser.add_argument("--left", required=True, help="Probe output directory for the reference side.")
    parser.add_argument("--right", required=True, help="Probe output directory for the comparison side.")
    parser.add_argument(
        "--output-dir",
        required=True,
        help="Directory that will receive the comparison summary and diff CSV files.",
    )
    args = parser.parse_args()

    left_dir = Path(args.left).resolve()
    right_dir = Path(args.right).resolve()
    output_dir = Path(args.output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    left_summary = load_summary(left_dir / "summary.txt")
    right_summary = load_summary(right_dir / "summary.txt")

    projected = compare_tables(
        left_dir / "backdrop_projected_vertices.csv",
        right_dir / "backdrop_projected_vertices.csv",
        "vertex_index",
        PROJECTED_NUMERIC_FIELDS,
        PROJECTED_ENUM_FIELDS,
    )
    triangles = compare_tables(
        left_dir / "backdrop_visible_triangles.csv",
        right_dir / "backdrop_visible_triangles.csv",
        "triangle_index",
        TRIANGLE_NUMERIC_FIELDS,
        TRIANGLE_ENUM_FIELDS,
    )
    raster_preview = compare_binary_file(
        left_dir / "backdrop_raster_preview.png",
        right_dir / "backdrop_raster_preview.png",
    )

    write_row_diffs(output_dir / "projected_vertex_diff.csv", projected)
    write_row_diffs(output_dir / "visible_triangle_diff.csv", triangles)
    summary_text = build_summary_text(left_dir, right_dir, left_summary, right_summary, projected, triangles, raster_preview)
    (output_dir / "comparison_summary.txt").write_text(summary_text, encoding="utf-8")
    print(summary_text)
    return 0


def load_summary(path: Path) -> dict[str, str]:
    summary: dict[str, str] = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        if "=" not in line:
            continue
        key, value = line.split("=", 1)
        summary[key.strip()] = value.strip()
    return summary


def load_csv_rows(path: Path) -> list[dict[str, str]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        return list(csv.DictReader(handle))


def compare_tables(
    left_path: Path,
    right_path: Path,
    key_field: str,
    numeric_fields: list[str],
    enum_fields: list[str],
) -> dict[str, object]:
    left_rows = load_csv_rows(left_path)
    right_rows = load_csv_rows(right_path)

    left_by_key = {row[key_field]: row for row in left_rows}
    right_by_key = {row[key_field]: row for row in right_rows}

    left_only = sorted(set(left_by_key) - set(right_by_key), key=sort_key)
    right_only = sorted(set(right_by_key) - set(left_by_key), key=sort_key)
    common_keys = sorted(set(left_by_key) & set(right_by_key), key=sort_key)

    numeric_metrics = {field: {"sum": 0.0, "max": 0.0, "count": 0} for field in numeric_fields}
    enum_mismatches = {field: 0 for field in enum_fields}
    row_diffs: list[dict[str, object]] = []

    for key in common_keys:
        left_row = left_by_key[key]
        right_row = right_by_key[key]
        row_diff: dict[str, object] = {key_field: key}
        max_numeric_diff = 0.0

        for field in numeric_fields:
            diff = abs(parse_float(left_row[field]) - parse_float(right_row[field]))
            row_diff[field + "_abs_diff"] = diff
            metrics = numeric_metrics[field]
            metrics["sum"] += diff
            metrics["count"] += 1
            if diff > metrics["max"]:
                metrics["max"] = diff
            if diff > max_numeric_diff:
                max_numeric_diff = diff

        enum_mismatch_count = 0
        for field in enum_fields:
            matches = left_row[field] == right_row[field]
            row_diff[field + "_matches"] = 1 if matches else 0
            if not matches:
                enum_mismatches[field] += 1
                enum_mismatch_count += 1

        row_diff["max_numeric_abs_diff"] = max_numeric_diff
        row_diff["enum_mismatch_count"] = enum_mismatch_count
        row_diffs.append(row_diff)

    return {
        "key_field": key_field,
        "left_count": len(left_rows),
        "right_count": len(right_rows),
        "common_count": len(common_keys),
        "left_only": left_only,
        "right_only": right_only,
        "numeric_metrics": numeric_metrics,
        "enum_mismatches": enum_mismatches,
        "row_diffs": sorted(row_diffs, key=lambda row: (-float(row["max_numeric_abs_diff"]), sort_key(row[key_field]))),
    }


def write_row_diffs(path: Path, comparison: dict[str, object]) -> None:
    row_diffs = comparison["row_diffs"]
    if not row_diffs:
        path.write_text("", encoding="utf-8")
        return

    first_row = row_diffs[0]
    fieldnames = list(first_row.keys())
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        for row in row_diffs:
            writer.writerow(row)


def build_summary_text(
    left_dir: Path,
    right_dir: Path,
    left_summary: dict[str, str],
    right_summary: dict[str, str],
    projected: dict[str, object],
    triangles: dict[str, object],
    raster_preview: dict[str, object],
) -> str:
    lines: list[str] = []
    lines.append(f"left_dir={left_dir}")
    lines.append(f"right_dir={right_dir}")
    lines.append(f"left_label={left_summary.get('probe_label', 'unknown')}")
    lines.append(f"right_label={right_summary.get('probe_label', 'unknown')}")
    lines.append("")
    lines.append("summary_differences:")

    summary_keys = sorted(set(left_summary) | set(right_summary))
    difference_count = 0
    for key in summary_keys:
        left_value = left_summary.get(key, "<missing>")
        right_value = right_summary.get(key, "<missing>")
        if left_value != right_value:
            difference_count += 1
            lines.append(f"  {key}: left={left_value} right={right_value}")
    if difference_count == 0:
        lines.append("  none")

    lines.append("")
    lines.extend(render_table_summary("projected_vertices", projected))
    lines.append("")
    lines.extend(render_table_summary("visible_triangles", triangles))
    lines.append("")
    lines.extend(render_binary_summary("backdrop_raster_preview_png", raster_preview))
    return "\n".join(lines) + "\n"


def render_table_summary(name: str, comparison: dict[str, object]) -> list[str]:
    lines: list[str] = []
    lines.append(f"{name}:")
    lines.append(
        "  row_counts: "
        f"left={comparison['left_count']} right={comparison['right_count']} common={comparison['common_count']}"
    )

    left_only = comparison["left_only"]
    right_only = comparison["right_only"]
    lines.append(f"  left_only_keys={len(left_only)}")
    if left_only:
        lines.append("  left_only_examples=" + ",".join(str(value) for value in left_only[:10]))
    lines.append(f"  right_only_keys={len(right_only)}")
    if right_only:
        lines.append("  right_only_examples=" + ",".join(str(value) for value in right_only[:10]))

    numeric_metrics = comparison["numeric_metrics"]
    for field in sorted(numeric_metrics):
        metrics = numeric_metrics[field]
        count = metrics["count"]
        mean = metrics["sum"] / count if count else 0.0
        lines.append(
            f"  {field}: mean_abs_diff={mean:.9f} max_abs_diff={metrics['max']:.9f}"
        )

    enum_mismatches = comparison["enum_mismatches"]
    for field in sorted(enum_mismatches):
        lines.append(f"  {field}: mismatches={enum_mismatches[field]}")

    top_rows = comparison["row_diffs"][:5]
    if top_rows:
        lines.append("  top_rows_by_numeric_diff:")
        key_field = comparison["key_field"]
        for row in top_rows:
            lines.append(
                f"    {key_field}={row[key_field]} "
                f"max_numeric_abs_diff={float(row['max_numeric_abs_diff']):.9f} "
                f"enum_mismatch_count={row['enum_mismatch_count']}"
            )
    return lines


def render_binary_summary(name: str, comparison: dict[str, object]) -> list[str]:
    lines: list[str] = []
    lines.append(f"{name}:")
    lines.append(f"  left_exists={comparison['left_exists']}")
    lines.append(f"  right_exists={comparison['right_exists']}")
    if comparison["left_exists"]:
        lines.append(f"  left_size={comparison['left_size']}")
        lines.append(f"  left_sha256={comparison['left_sha256']}")
    if comparison["right_exists"]:
        lines.append(f"  right_size={comparison['right_size']}")
        lines.append(f"  right_sha256={comparison['right_sha256']}")
    lines.append(f"  binary_equal={comparison['binary_equal']}")
    return lines


def compare_binary_file(left_path: Path, right_path: Path) -> dict[str, object]:
    left_exists = left_path.exists()
    right_exists = right_path.exists()
    result: dict[str, object] = {
        "left_exists": left_exists,
        "right_exists": right_exists,
        "binary_equal": False,
    }
    if left_exists:
        left_bytes = left_path.read_bytes()
        result["left_size"] = len(left_bytes)
        result["left_sha256"] = hashlib.sha256(left_bytes).hexdigest()
    if right_exists:
        right_bytes = right_path.read_bytes()
        result["right_size"] = len(right_bytes)
        result["right_sha256"] = hashlib.sha256(right_bytes).hexdigest()
    if left_exists and right_exists:
        result["binary_equal"] = result["left_sha256"] == result["right_sha256"]
    return result


def parse_float(value: str) -> float:
    if value == "NaN":
        return 0.0
    if value == "Infinity":
        return math.inf
    if value == "-Infinity":
        return -math.inf
    return float(value)


def sort_key(value: str) -> tuple[int, str]:
    try:
        return (0, f"{int(value):020d}")
    except ValueError:
        return (1, value)


if __name__ == "__main__":
    raise SystemExit(main())
