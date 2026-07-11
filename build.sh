#!/usr/bin/env bash
# build.sh — Build mbox2eml
# mbox2eml is a fast multi-threaded C++ tool that extracts messages from mbox files into individual .eml files
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Building mbox2eml (via Makefile)..."
make

echo "Done."
