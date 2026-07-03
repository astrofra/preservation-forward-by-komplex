#!/usr/bin/env sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT_DIR="$SCRIPT_DIR"
cd "$ROOT_DIR"

BUILD_DIR="${BUILD_DIR:-cpp-offline/build}"

if [ "${1:-}" = "" ]; then
    CONFIG="${CONFIG:-Release}"
else
    CONFIG="$1"
fi

case "$CONFIG" in
    Release|Debug|RelWithDebInfo|MinSizeRel)
        ;;
    *)
        printf 'Usage: %s [Release|Debug|RelWithDebInfo|MinSizeRel]\n' "$(basename "$0")" >&2
        exit 1
        ;;
esac

if ! command -v cmake >/dev/null 2>&1; then
    printf 'CMake was not found in PATH.\n' >&2
    exit 1
fi

printf '[1/2] Configure CMake\n'
cmake -S cpp-offline -B "$BUILD_DIR"

printf '[2/2] Build cpp-offline (%s)\n' "$CONFIG"
cmake --build "$BUILD_DIR" --config "$CONFIG"

FORWARD_EXPORT="$BUILD_DIR/$CONFIG/forward-export"
if [ -x "$FORWARD_EXPORT" ]; then
    printf 'Build complete:\n'
    printf '  %s\n' "$FORWARD_EXPORT"
    exit 0
fi

FORWARD_EXPORT="$BUILD_DIR/forward-export"
if [ -x "$FORWARD_EXPORT" ]; then
    printf 'Build complete:\n'
    printf '  %s\n' "$FORWARD_EXPORT"
    exit 0
fi

FORWARD_EXPORT="$BUILD_DIR/$CONFIG/forward-export.exe"
if [ -f "$FORWARD_EXPORT" ]; then
    printf 'Build complete:\n'
    printf '  %s\n' "$FORWARD_EXPORT"
    exit 0
fi

FORWARD_EXPORT="$BUILD_DIR/forward-export.exe"
if [ -f "$FORWARD_EXPORT" ]; then
    printf 'Build complete:\n'
    printf '  %s\n' "$FORWARD_EXPORT"
    exit 0
fi

printf 'Build complete, but forward-export was not found in the expected locations.\n' >&2
printf 'Checked:\n' >&2
printf '  %s/%s/forward-export\n' "$BUILD_DIR" "$CONFIG" >&2
printf '  %s/forward-export\n' "$BUILD_DIR" >&2
printf '  %s/%s/forward-export.exe\n' "$BUILD_DIR" "$CONFIG" >&2
printf '  %s/forward-export.exe\n' "$BUILD_DIR" >&2
exit 1
