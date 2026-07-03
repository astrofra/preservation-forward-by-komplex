#!/usr/bin/env sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT_DIR=$(CDPATH= cd -- "$SCRIPT_DIR/../.." && pwd)
cd "$ROOT_DIR"

BUILD_DIR="${BUILD_DIR:-cpp-offline/build}"
CONFIG="${CONFIG:-Release}"
OUTPUT_DIR="${OUTPUT_DIR:-cpp-offline/output-full-current}"
TEMP_DIR="${TEMP_DIR:-${OUTPUT_DIR}-tmp}"
INTRO_OUTPUT="${TEMP_DIR}/intro"
SAARI_OUTPUT="${TEMP_DIR}/saari"
KUKOT_OUTPUT="${TEMP_DIR}/kukot"
MAKU_OUTPUT="${TEMP_DIR}/maku"

INTRO_END_POSITION="${INTRO_END_POSITION:-0x1024}"
INTRO_POST_ROLL_FRAMES="${INTRO_POST_ROLL_FRAMES:-12}"

SAARI_END_POSITION="${SAARI_END_POSITION:-0x0700}"
SAARI_POST_ROLL_FRAMES="${SAARI_POST_ROLL_FRAMES:-0}"

KUKOT_END_POSITION="${KUKOT_END_POSITION:-0x0D00}"
KUKOT_POST_ROLL_FRAMES="${KUKOT_POST_ROLL_FRAMES:-0}"

MAKU_END_POSITION="${MAKU_END_POSITION:-0x1000}"
MAKU_POST_ROLL_FRAMES="${MAKU_POST_ROLL_FRAMES:-0}"

PYTHON="${PYTHON:-python3}"

resolve_exporter() {
    if [ -x "$BUILD_DIR/forward-export" ]; then
        printf '%s\n' "$BUILD_DIR/forward-export"
        return 0
    fi
    if [ -x "$BUILD_DIR/$CONFIG/forward-export" ]; then
        printf '%s\n' "$BUILD_DIR/$CONFIG/forward-export"
        return 0
    fi
    if [ -x "$BUILD_DIR/forward-export.exe" ]; then
        printf '%s\n' "$BUILD_DIR/forward-export.exe"
        return 0
    fi
    if [ -x "$BUILD_DIR/$CONFIG/forward-export.exe" ]; then
        printf '%s\n' "$BUILD_DIR/$CONFIG/forward-export.exe"
        return 0
    fi
    return 1
}

if [ -d "$TEMP_DIR" ]; then
    rm -rf "$TEMP_DIR"
fi

printf '[1/8] Configure CMake\n'
cmake -S cpp-offline -B "$BUILD_DIR"

printf '[2/8] Build exporter\n'
cmake --build "$BUILD_DIR" --config "$CONFIG"

EXPORTER=$(resolve_exporter) || {
    printf 'Unable to locate built forward-export under %s\n' "$BUILD_DIR" >&2
    exit 1
}

printf '[3/8] Export intro segment\n'
printf '        end=%s post_roll_frames=%s\n' "$INTRO_END_POSITION" "$INTRO_POST_ROLL_FRAMES"
"$EXPORTER" \
  --sequence intro \
  --output "$INTRO_OUTPUT" \
  --until-song-position "$INTRO_END_POSITION" \
  --post-roll-frames "$INTRO_POST_ROLL_FRAMES"

printf '[4/8] Export saari segment\n'
printf '        end=%s post_roll_frames=%s\n' "$SAARI_END_POSITION" "$SAARI_POST_ROLL_FRAMES"
"$EXPORTER" \
  --sequence saari \
  --output "$SAARI_OUTPUT" \
  --until-song-position "$SAARI_END_POSITION" \
  --post-roll-frames "$SAARI_POST_ROLL_FRAMES"

printf '[5/8] Export kukot segment\n'
printf '        end=%s post_roll_frames=%s\n' "$KUKOT_END_POSITION" "$KUKOT_POST_ROLL_FRAMES"
"$EXPORTER" \
  --sequence kukot \
  --output "$KUKOT_OUTPUT" \
  --until-song-position "$KUKOT_END_POSITION" \
  --post-roll-frames "$KUKOT_POST_ROLL_FRAMES"

printf '[6/8] Export maku segment\n'
printf '        end=%s post_roll_frames=%s\n' "$MAKU_END_POSITION" "$MAKU_POST_ROLL_FRAMES"
"$EXPORTER" \
  --sequence maku \
  --output "$MAKU_OUTPUT" \
  --until-song-position "$MAKU_END_POSITION" \
  --post-roll-frames "$MAKU_POST_ROLL_FRAMES"

command -v "$PYTHON" >/dev/null 2>&1 || {
    printf 'Required interpreter not found: %s\n' "$PYTHON" >&2
    exit 1
}

printf '[7/8] Merge current full output\n'
"$PYTHON" cpp-offline/scripts/merge_current_full_outputs.py \
  --output-dir "$OUTPUT_DIR" \
  --intro-dir "$INTRO_OUTPUT" \
  --saari-dir "$SAARI_OUTPUT" \
  --kukot-dir "$KUKOT_OUTPUT" \
  --maku-dir "$MAKU_OUTPUT" \
  --fps 50 \
  --sample-rate 22050

if [ -d "$TEMP_DIR" ]; then
    rm -rf "$TEMP_DIR"
fi

if ! command -v ffmpeg >/dev/null 2>&1; then
    printf 'Export complete: %s\n' "$OUTPUT_DIR"
    exit 0
fi

printf '[8/8] Mux master and h264 copies\n'
sh cpp-offline/scripts/mux_master.sh "$OUTPUT_DIR" "$OUTPUT_DIR/forward_full_current_master.mkv" 50
sh cpp-offline/scripts/mux_h264.sh "$OUTPUT_DIR" "$OUTPUT_DIR/forward_full_current_h264.mp4" 50

printf 'Export complete: %s\n' "$OUTPUT_DIR"
