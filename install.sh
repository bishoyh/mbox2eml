#!/usr/bin/env bash
# install.sh — Build and install mbox2eml into PREFIX/bin (default /usr/local).
# mbox2eml is a fast multi-threaded C++ tool that extracts messages from mbox
# files into individual .eml files.
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

PREFIX="${PREFIX:-/usr/local}"

echo "Building mbox2eml..."
make

echo "Installing to $PREFIX/bin (may prompt for sudo)..."
# Try an unprivileged install first; fall back to sudo for system prefixes.
if make install PREFIX="$PREFIX" 2>/dev/null; then
  echo "Installed to $PREFIX/bin/mbox2eml"
else
  sudo make install PREFIX="$PREFIX"
  echo "Installed to $PREFIX/bin/mbox2eml"
fi
echo "Done."
