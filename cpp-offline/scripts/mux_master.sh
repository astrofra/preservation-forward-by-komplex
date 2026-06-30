#!/usr/bin/env sh
set -eu

OUTPUT_DIR="${1:-output}"
OUTPUT_FILE="${2:-$OUTPUT_DIR/forward_master.mkv}"
FPS="${3:-50}"

ffmpeg -y \
  -framerate "$FPS" \
  -i "$OUTPUT_DIR/frames/frame_%06d.tga" \
  -i "$OUTPUT_DIR/audio/forward.wav" \
  -c:v ffv1 \
  -level 3 \
  -g 1 \
  -c:a pcm_s16le \
  "$OUTPUT_FILE"
