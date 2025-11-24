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
