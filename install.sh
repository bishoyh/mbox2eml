#!/usr/bin/env bash
# install.sh — Install mbox2eml locally
# mbox2eml is a fast multi-threaded C++ tool that extracts messages from mbox files into individual .eml files
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Installing mbox2eml..."
make install 2>/dev/null || (sudo cp "$SCRIPT_DIR/mbox2eml" /usr/local/bin/ && echo "Installed to /usr/local/bin/mbox2eml") || echo "Install failed"
echo "Done."
