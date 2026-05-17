#!/usr/bin/env bash
# publish.sh — Build, install, and release mbox2eml
# mbox2eml is a fast multi-threaded C++ tool that extracts messages from mbox files into individual .eml files
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Building..."
bash "$SCRIPT_DIR/build.sh"

echo "Installing..."
bash "$SCRIPT_DIR/install.sh"

echo "Tagging next version..."
uvx gitnextver@latest

echo "Done."
