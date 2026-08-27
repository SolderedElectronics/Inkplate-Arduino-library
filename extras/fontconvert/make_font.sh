#!/usr/bin/env bash
# make_font.sh - convenience wrapper around make_font.py.
#
# Creates a throwaway virtualenv next to this script (so nothing gets
# installed into your system/global Python), installs the one dependency
# (freetype-py) into it if it isn't already there, then runs make_font.py
# with whatever arguments you passed. Safe to re-run - it reuses the venv
# and skips reinstalling if freetype-py is already present.
#
# Usage (same arguments as make_font.py itself):
#   ./make_font.sh /path/to/Font.ttf ä ö ü ß Ä Ö Ü
#   ./make_font.sh /path/to/Font.ttf š ć č Č đ ž --size 24 --out MyFont24pt7b.h
#
# Requires: python3 with the 'venv' module (standard on macOS/Linux; on
# Debian/Ubuntu you may need `apt install python3-venv` first). On Windows,
# use make_font.bat instead (same behavior, no bash/WSL needed).
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="$SCRIPT_DIR/.venv"

if ! command -v python3 >/dev/null 2>&1; then
    echo "python3 not found on PATH - install Python 3 first." >&2
    exit 1
fi

if [ ! -x "$VENV_DIR/bin/python3" ]; then
    echo "Setting up a local virtualenv at $VENV_DIR (one-time)..." >&2
    python3 -m venv "$VENV_DIR"
fi

if ! "$VENV_DIR/bin/python3" -c "import freetype" >/dev/null 2>&1; then
    echo "Installing freetype-py into the venv (one-time)..." >&2
    "$VENV_DIR/bin/pip" install --quiet --upgrade pip freetype-py
fi

exec "$VENV_DIR/bin/python3" "$SCRIPT_DIR/make_font.py" "$@"
