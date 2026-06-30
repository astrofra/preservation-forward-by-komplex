#!/usr/bin/env sh
set -eu

OUTPUT_DIR="${1:-output}"
OUTPUT_FILE="${2:-$OUTPUT_DIR/forward_h264.mp4}"
FPS="${3:-50}"

ffmpeg -y \
  -framerate "$FPS" \
  -i "$OUTPUT_DIR/frames/frame_%06d.tga" \
  -i "$OUTPUT_DIR/audio/forward.wav" \
  -c:v libx264 \
  -preset slow \
  -crf 12 \
  -pix_fmt yuv420p \
  -c:a aac \
  -b:a 192k \
  "$OUTPUT_FILE"
