#!/usr/bin/env bash
set -euo pipefail

# Ensure UTF-8 environment for MSYS2/MinGW shells
export LANG="zh_CN.UTF-8"
export LC_ALL="zh_CN.UTF-8"
export MSYS_UTF8=1

# Usage: build_current_file.sh [path/to/file.c]
# If an argument is provided and exists, build it. Otherwise present a selection menu for .c files in the workspace.

WORKSPACE_DIR="${PWD}"
ARG_FILE="${1-}" 

choose_file() {
  # Build an array of .c files
  mapfile -t allfiles < <(printf "%s\n" "$WORKSPACE_DIR"/*.c)
  filtered=()
  for f in "${allfiles[@]}"; do
    [ -f "$f" ] || continue
    filtered+=("$f")
  done
  if [ ${#filtered[@]} -eq 0 ]; then
    echo "No .c files found in ${WORKSPACE_DIR}" >&2
    exit 1
  fi

  # Present a numbered menu and read from the real terminal (/dev/tty) so input echoes correctly
  echo "Select a C file to build:"
  for i in "${!filtered[@]}"; do
    printf "%3d) %s\n" $((i+1)) "${filtered[$i]}"
  done
  while true; do
    if ! read -r -p "Enter number (or Ctrl-C to cancel): " choice </dev/tty; then
      echo "Selection cancelled" >&2
      exit 1
    fi
    if [[ "$choice" =~ ^[0-9]+$ ]] && [ "$choice" -ge 1 ] && [ "$choice" -le ${#filtered[@]} ]; then
      idx=$((choice-1))
      echo "${filtered[$idx]}"
      return 0
    fi
    echo "Invalid selection" >&2
  done
}

if [ -n "$ARG_FILE" ] && [ -f "$ARG_FILE" ]; then
  SOURCE="$ARG_FILE"
else
  # If running non-interactively (no TTY), pick the first .c file automatically.
  if [ ! -t 0 ]; then
    for f in "$WORKSPACE_DIR"/*.c; do
      if [ -f "$f" ]; then
        SOURCE="$f"
        break
      fi
    done
    if [ -z "${SOURCE-}" ]; then
      echo "No .c files found in ${WORKSPACE_DIR}" >&2
      exit 1
    fi
  else
    SOURCE=$(choose_file)
  fi
fi

basename_no_ext=$(basename "$SOURCE" .c)
out="$WORKSPACE_DIR/output/${basename_no_ext}.exe"
mkdir -p "$(dirname "$out")"
echo "Compiling $SOURCE -> $out"
# Detect SDL usage and gather flags (use pkg-config when available)
sdl_flags=""
if grep -q "#include *<SDL.h>" "$SOURCE" 2>/dev/null || grep -q "#include *<SDL2/SDL.h>" "$SOURCE" 2>/dev/null || grep -q "#include *<SDL_image.h>" "$SOURCE" 2>/dev/null || grep -q "#include *<SDL_mixer.h>" "$SOURCE" 2>/dev/null; then
  if command -v pkg-config >/dev/null 2>&1; then
    sdl_flags="$(pkg-config --cflags --libs sdl2 SDL2_image SDL2_mixer 2>/dev/null || true)"
  else
    # Fallback to common MSYS2 paths
    sdl_flags="-I\"C:/msys64/mingw64/include/SDL2\" -L\"C:/msys64/mingw64/lib\" -lSDL2 -lSDL2_image -lSDL2_mixer"
  fi
fi

gcc -Wall -Wextra -g3 "$SOURCE" $sdl_flags -o "$out"
rc=$?
if [ $rc -ne 0 ]; then
  echo "Compilation failed with exit code $rc" >&2
  exit $rc
fi
echo "Built: $out"
# write a small marker file so other tools can verify which source produced this exe
echo "$SOURCE" > "$out.built_from"
date -u +"%Y-%m-%dT%H:%M:%SZ" > "$out.built_at"

exit 0
