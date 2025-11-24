SDL2 使用与构建说明 (MSYS2 / mingw64)

概览
- 你已在 `C:/msys64/mingw64` 安装好 SDL2，SDL2_image，SDL2_mixer，并已把运行时 DLL 放到项目的 `output/` 目录。
- 仓库已添加测试程序 `main_sdl.c`，及若干 VS Code 任务来编译与复制 DLL：
  - `build sdl`：使用 `pkg-config` 在 MSYS2 环境里编译 `main.c` -> `output/main_sdl.exe` 并复制常见 DLL。
  - `build sdl win`：使用显式路径（`C:/msys64/mingw64`）编译 `main_sdl.c` -> `output/main_sdl_win.exe`，适用于在非 pkg-config 环境下的命令行或 PowerShell。
  - `copy sdl dlls`：单独把 `/mingw64/bin` 的 SDL 相关 DLL 复制到 `output/`。

如何在 MSYS2 (mingw64) 下验证（已测试）
1. 打开 "MSYS2 MinGW 64-bit" shell。
2. 在项目根运行：
```bash
gcc -Wall -Wextra -g3 main_sdl.c -o output/main_sdl.exe $(pkg-config --cflags --libs sdl2 SDL2_image SDL2_mixer)
./output/main_sdl.exe
```
期望输出：
```
SDL, SDL_image and SDL_mixer initialized successfully.
```

在 Windows PowerShell / CMD 下的等效命令（不依赖 pkg-config）
- 编译（PowerShell 或 CMD）：下面是更完整的显式链接命令，包含常见的运行时依赖库（ogg/vorbis/opus/png/jpeg）：
```
gcc -IC:/msys64/mingw64/include/SDL2 -LC:/msys64/mingw64/lib main_sdl.c -o output/main_sdl_win.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -logg -lvorbis -lopus -lpng16 -ljpeg
```
（这是仓库任务 `build sdl win` 中使用的完整链接行。）
- 复制 DLL（PowerShell）：
```powershell
Copy-Item C:\msys64\mingw64\bin\SDL2.dll -Destination .\output\ -Verbose
Copy-Item C:\msys64\mingw64\bin\SDL2_image.dll -Destination .\output\ -Verbose
Copy-Item C:\msys64\mingw64\bin\SDL2_mixer.dll -Destination .\output\ -Verbose
# 如需其它依赖 (libogg, libvorbis, libopus, libpng, libjpeg)，也一并复制
```

VS Code 中的使用（已加入任务与启动配置）
- 直接用任务：Ctrl+Shift+P -> Tasks: Run Task -> 选择 `build sdl`（在 MSYS2 shell 环境下）或 `build sdl win`（显式路径编译）。
- 若要运行调试/运行配置：选择 `Run main_sdl.exe (Build SDL then Run)`，VS Code 会先执行 `build sdl`，然后运行 `output/main_sdl.exe`。

故障排查要点
- 如果 `pkg-config` 报错找不到库，先在 MSYS2 MinGW64 下运行 `pkg-config --cflags --libs sdl2 SDL2_image SDL2_mixer` 看输出。
- 链接错误时，尝试使用 `build sdl win` 的显式路径命令并检查 `C:/msys64/mingw64/lib` 下是否存在 `libSDL2.a` 或 `libSDL2.dll.a`。
- 运行时报缺 DLL：把对应的 DLL（从 `C:/msys64/mingw64/bin`）复制到 `output/`。

如果你希望我把 `build sdl win` 的链接选项扩展为包含所有可能的依赖库（libogg/libvorbis/libopus/libpng/libjpeg 等），我可以把命令改成更完整的一行并提交到 `tasks.json`。
- 关于中文/UTF-8 编码 (已配置)
- 工作区已启用 UTF-8：`.vscode/settings.json` 设置了 `files.encoding = utf8`，并为集成终端注入了 `LANG=zh_CN.UTF-8`、`LC_ALL=zh_CN.UTF-8` 和 `MSYS_UTF8=1`。这能让 MSYS2 / MinGW 的 bash 输出使用 UTF-8 编码，减少中文乱码。
- 任务与调试配置也设置了相同的环境变量（`tasks.json` 与 `.vscode/launch.json` 的 `environment`），并且构建脚本在运行时会导出这些变量，保证 subprocess 使用 UTF-8。
- 如果你仍在 Windows 外部控制台（PowerShell/CMD）中看到乱码，确保你的控制台使用 UTF-8 代码页：在 PowerShell 中运行 `chcp 65001`，或在程序中调用 `SetConsoleOutputCP(65001)`（仓库中已有 `setConsoleOutputCPToUTF8()` 建议函数）。

平台控制台助手
- 新增模块 `platform_console.h` / `platform_console.c`，包含 `setConsoleOutputCPToUTF8()` 的跨平台实现：
  - 在 Windows 上调用 `SetConsoleOutputCP(CP_UTF8)` / `SetConsoleCP(CP_UTF8)`。
  - 在 POSIX（Linux/macOS）上调用 `setlocale(LC_ALL, "")` 以使用环境定义的编码（通常设置为 UTF-8）。
  - 代码示例：在你的 `main()` 开头调用 `setConsoleOutputCPToUTF8();` 来统一控制台编码行为。

这使得代码在 Windows 下能正确设置控制台为 UTF-8，同时在其他平台也能优雅降级。
