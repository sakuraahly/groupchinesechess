#!/usr/bin/env bash
set -euo pipefail
export LANG=${LANG:-zh_CN.UTF-8}
export LC_ALL=${LC_ALL:-zh_CN.UTF-8}
export MSYS_UTF8=1

# Accept optional workspace path as first arg; fallback to PWD
ws="${1:-$PWD}"
# Convert Windows path to POSIX if cygpath available
if command -v cygpath >/dev/null 2>&1; then
  ws="$(cygpath -u "$ws" 2>/dev/null || printf "%s" "$ws")"
fi
cd "$ws" || { echo "cd failed: $ws"; exit 1; }

mkdir -p ./output

# Use shell globbing but enable nullglob so no-match produces empty array
shopt -s nullglob || true
files=( ./*.c )
if [ "${#files[@]}" -eq 0 ]; then
  echo "No .c files found in $ws"
  exit 0
fi

for src in "${files[@]}"; do
  base=$(basename "$src")
  # Skip helper-only sources that shouldn't produce standalone exes
  if [ "$base" = "platform_console.c" ]; then
    echo "Skipping helper-only source: $src"
    continue
  fi
  exe=./output/$(basename "$src" .c).exe
  echo "Compiling $src -> $exe"

  # Build arrays for gcc arguments to safely handle spaces
  extra_sources_arr=()
  if [ "$base" != "platform_console.c" ] && ( grep -q '#include *"platform_console.h"' "$src" 2>/dev/null || grep -q 'setConsoleEncodingFromEnv' "$src" 2>/dev/null ); then
    if [ -f "./platform_console.c" ]; then
      extra_sources_arr+=("./platform_console.c")
    fi
  fi

  # Detect SDL usage (includes, filenames, or common SDL symbols)
  is_sdl=0
  if grep -Eq "#include *<(SDL|SDL2/SDL|SDL2/SDL_image|SDL2/SDL_mixer|SDL_image.h|SDL_mixer.h)>" "$src" 2>/dev/null || \
     grep -Eq "SDL_Init|IMG_Init|Mix_OpenAudio|SDL_Quit|IMG_Quit|Mix_Quit" "$src" 2>/dev/null || \
     [[ "${base,,}" == *sdl*.c ]]; then
    is_sdl=1
  fi

  sdl_flags_arr=()
  if [ "$is_sdl" -eq 1 ]; then
    if command -v pkg-config >/dev/null 2>&1; then
      # Prefer pkg-config; split its output into array
      pkg_out="$(pkg-config --cflags --libs sdl2 SDL2_image SDL2_mixer 2>/dev/null || true)"
      if [ -n "$pkg_out" ]; then
        read -r -a sdl_flags_arr <<< "$pkg_out"
      fi
    fi
    # Fallback to common MSYS2 MinGW-w64 locations if pkg-config not available
    if [ ${#sdl_flags_arr[@]} -eq 0 ]; then
      if [ -d "/mingw64/include/SDL2" ] && [ -d "/mingw64/lib" ]; then
        sdl_flags_arr+=("-I/mingw64/include/SDL2" "-L/mingw64/lib" "-lSDL2" "-lSDL2_image" "-lSDL2_mixer")
      elif [ -d "/usr/include/SDL2" ]; then
        sdl_flags_arr+=("-I/usr/include/SDL2" "-lSDL2" "-lSDL2_image" "-lSDL2_mixer")
      fi
    fi
  fi

  # Construct gcc argument list and run
  gcc_args=("-Wall" "-Wextra" "-g3" "$src")
  if [ ${#extra_sources_arr[@]} -ne 0 ]; then
    gcc_args+=("${extra_sources_arr[@]}")
  fi
  if [ ${#sdl_flags_arr[@]} -ne 0 ]; then
    gcc_args+=("${sdl_flags_arr[@]}")
  fi

  gcc "${gcc_args[@]}" -o "$exe" || { echo "Compile failed: $src"; exit 1; }
done

# Copy DLLs if present
if [ -x ".vscode/scripts/copy_sdl_dlls.sh" ] || [ -f ".vscode/scripts/copy_sdl_dlls.sh" ]; then
  ./.vscode/scripts/copy_sdl_dlls.sh || true
fi

echo "build_all.sh finished"
