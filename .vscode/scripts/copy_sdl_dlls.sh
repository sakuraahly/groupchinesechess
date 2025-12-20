#!/usr/bin/env bash
set -euo pipefail
 
# Ensure UTF-8 environment for MSYS2/MinGW shells
export LANG="zh_CN.UTF-8"
export LC_ALL="zh_CN.UTF-8"
export MSYS_UTF8=1
mkdir -p "${PWD}/output"
# List of patterns to copy; only copy if destination missing
patterns=( "/mingw64/bin/SDL2*.dll" "/mingw64/bin/libSDL2_image*" "/mingw64/bin/libSDL2_mixer*" "/mingw64/bin/libopus*" "/mingw64/bin/libvorbis*" "/mingw64/bin/libogg*" "/mingw64/bin/libpng16-16.dll" "/mingw64/bin/libjpeg-*.dll" )
for p in "${patterns[@]}"; do
  for f in $p; do
    [ -e "$f" ] || continue
    dest="${PWD}/output/$(basename "$f")"
    if [ ! -e "$dest" ]; then
      cp -v "$f" "$dest"
    else
      echo "Skipping existing: $(basename "$f")"
    fi
  done
done
