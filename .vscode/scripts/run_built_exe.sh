#!/usr/bin/env bash
set -euo pipefail

# Usage: run_built_exe.sh path/to/exe
# Verifies .built_from marker (if present) and runs the exe in the current terminal.

EXE_PATH="${1-}"
if [ -z "$EXE_PATH" ]; then
  echo "Usage: $0 path/to/exe" >&2
  exit 2
fi
if [ ! -f "$EXE_PATH" ]; then
  echo "Executable not found: $EXE_PATH" >&2
  exit 3
fi

if [ -f "$EXE_PATH.built_from" ]; then
  src=$(cat "$EXE_PATH.built_from")
  echo "Executable was built from: $src"
else
  echo "No built_from marker for $EXE_PATH"
fi

echo "Running: $EXE_PATH"
"$EXE_PATH"
