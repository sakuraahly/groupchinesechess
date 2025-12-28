## Quick notes for AI coding agents (project: groupchinesechess)

Keep this short — key facts an AI needs to be productive in this repo.

- **Purpose:** small C-language workspace containing standalone example programs and a `src/` set of modules implementing Chinese-chess logic and display.

- **High-level architecture:**
  - Top-level examples: `main.c`, `learning.c`, `test.c` — each is a standalone program with its own `main()` (runnable as an exe).
  - Library-like code in `src/`: `game.c`, `displayinterface.c`, `chess_move.c`, `chess_database.c` and headers in `src/include/`. These files implement the game rules, moves, database and display glue.
  - Assets & outputs: `res/` (fonts/images/music), `output/` (built exes), helper scripts in `scripts/` and `.vscode/scripts/`.

- **Branch & PR rules:**
  - Do not push directly to `main` without captain approval. Always create feature branches and open PRs for review; document cross-file renames.

- **Build / run (explicit):**
  - Manual GCC (Bash + MinGW/GCC):
    - `gcc -o output/main.exe main.c`
    - `gcc -o output/learning.exe learning.c`
  - Recommended VS Code tasks (see `.vscode/tasks`):
    - `Build SDL File` — builds the active file with SDL linking if SDL headers detected.
    - `build sdl` / `build sdl win` — preconfigured SDL builds for `main.c` variants.
    - `build all` — runs `./.vscode/scripts/build_all.sh ${workspaceFolder}`.
  - Runtime: run the produced exe from `output/` (e.g. `./output/main.exe`).

- **Platform & encoding:**
  - Windows-focused code (uses `windows.h`). Console UTF-8 behavior is centralized — prefer editing `setConsoleOutputCPToUTF8()` (used in `main.c`/`test.c`) instead of adding raw `SetConsole*` calls.

- **Project-specific conventions:**
  - Files are frequently standalone demos. Keep this pattern unless you deliberately refactor into shared libraries; when you do, add build instructions (Makefile or README changes).
  - Maintain existing function names within a file (naming inconsistency is normal here). If you rename across files, update all call sites and document in PR.
  - Randomized algorithms exist (e.g., `randomPartition` in `main.c`); use explicit `srand()` seeds for reproducible tests.

- **Integration points & external deps:**
  - SDL2: optional — linked via `pkg-config` in tasks or explicit MinGW flags; DLLs are copied by `.vscode/scripts/copy_sdl_dlls.sh`.
  - Resources: `res/` contains images/fonts/music used by SDL/display code.

- **Key files to inspect first:**
  - `src/game.c` — main game loop / orchestration
  - `src/chess_move.c` and `src/chess_database.c` — rules and state
  - `src/displayinterface.c` — rendering / console helpers
  - Top-level `main.c`, `learning.c`, `test.c` — usage examples and encoding handling

- **Testing & debugging tips:**
  - Build with the appropriate task or GCC command, then run `./output/<exe>.exe`.
  - Add small harnesses in the same `.c` file for quick local checks.
  - Use MinGW GDB or print-based debugging; many components rely on stdout inspection.

- **PR checklist for AI edits:**
  - Keep changes minimal and focused per PR.
  - If adding shared headers/modules, include compile/run instructions in `README.md` or a Makefile and update `.vscode/tasks` if needed.
  - Avoid committing built binaries; document outputs instead.

Reference files: `README.md`, `main.c`, `learning.c`, `test.c`, `src/game.c`, `src/chess_move.c`, `src/chess_database.c`, `src/displayinterface.c`, `.vscode/scripts/`.

If anything important is missing (preferred MinGW distribution, toolchain PATH, or captain contact), tell me and I'll integrate it.
## Quick notes for AI coding agents (project: groupchinesechess)

Keep this short and focused — what an AI needs to be productive when editing or extending this repo.

- **Repository purpose:** small C-language group workspace containing standalone example programs (`main.c`, `learning.c`, `test.c`) that demonstrate algorithms (quick sort, string ops) and console handling on Windows.
- **Branch rule:** do not change `main` branch without the captain's permission (see `README.md`). Prefer working on feature branches and request review.

- **Build / run (discoverable):** prebuilt EXEs are in the repo root (`main.exe`, `learning.exe`, `test.exe`) and a `build/` folder exists.
  - Typical compile (from Bash on Windows with MinGW/GCC):
    - `gcc -o main.exe main.c`
    - `gcc -o learning.exe learning.c`
    - `gcc -o test.exe test.c`
  - Run with: `./main.exe` (or `./learning.exe`, `./test.exe`). These were used by the developer when running locally.
  - Note: `SDL2.dll` and `SDL2_ttf.dll` are present but not referenced by the current C sources; link only if adding SDL code.

- **Platform specifics:** code includes `windows.h` (in `main.c` and `test.c`) and uses `SetConsoleOutputCP(65001)` to force UTF-8 output. When changing console/encoding behavior, prefer modifying `setConsoleOutputCPToUTF8()` rather than sprinkling `SetConsole...` calls.

- **File-level patterns & conventions:**
  - Each `.c` file currently is a standalone program with its own `main()` (no shared library). Expect PRs that either add new standalone examples or refactor common logic into `.h`/`.c` modules.
  - Naming is inconsistent across files: `quicksort` (lowercase) in `learning.c` vs `quickSort` (camelCase) in `main.c`. Preserve existing function names when editing that specific file; when refactoring across files, document the rename in your PR.
  - Randomized quicksort is used in `main.c` (`randomPartition`) to avoid worst-case inputs — when adding tests or changes, keep test seeds explicit using `srand()` if reproducibility is required.

- **Coding style observed (useful for edits):**
  - Plain C (C89/C99 compatible constructs) with simple helper functions (swap, partition, printArray).
  - Minimal comments in Chinese/English in some files — preserve meaning when rewording.

- **What to look for when editing or adding code:**
  - Keep standalone examples runnable with `gcc` as above.
  - If you introduce shared headers (`.h`), add a small Makefile or document compile commands in `README.md`.
  - For console text, use `setConsoleOutputCPToUTF8()` for Windows to centralize encoding handling.
  - If adding SDL usage, explain linking and runtime DLL dependency (where to place `SDL2.dll`).

- **Testing & debugging hints:**
  - Reproduce runs locally by compiling the relevant `.c` file and running the produced `.exe`.
  - For algorithm changes (sorts), add a small harness in the same file or a new test file that prints results to stdout; existing tests are simple, manual-run based.

- **PR guidance for AI changes:**
  - Keep changes minimal and well-justified. If renaming a function across files, include a short note and update all call sites.
  - Do not push changes to `main` without approval. Create a feature branch and open a PR.

Reference files: `README.md`, `main.c`, `learning.c`, `test.c`, `build/`.

If anything here is unclear or you want the agent to follow a stricter style guide (naming, formatting, or automated build), tell me what to enforce and I will update this file accordingly.
