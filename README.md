```markdown
This our c language group collaborative work repository.
Do not change the main branch over the captain`s permission.//We just want to make it smoothly to go on.

//everyone is suppoesed to follow the main template.
```

## 构建与任务（MSYS2 / VS Code）

- 一键构建（推荐）: 项目已提供一个可靠的构建脚本 `./.vscode/scripts/build_all.sh`，它会在 MSYS2/MinGW bash 环境下把工作区根目录下的所有 `*.c` 编译到 `./output/`，并在结束时拷贝所需的 SDL DLL（只拷贝缺失的 DLL）。

- 在终端运行（MSYS bash）：
	```bash
	cd /d/MY_CODING_PROGRAM/C/groupchinesechess   # 根据你的 MSYS 路径替换
	chmod +x ./.vscode/scripts/build_all.sh
	./.vscode/scripts/build_all.sh "${PWD}"
	```

- 在 VS Code 中运行: 已将 `build all` 任务绑定为调用上述脚本。可通过 `Terminal -> Run Task... -> build all` 来触发（任务在工作区根目录执行脚本，因此不必在 JSON 中嵌入 Windows 路径）。

- 输出位置: 编译产物放在 `./output/` 目录（如需改为 `build/`，可告诉我，我会替换脚本与任务配置）。

- 控制台编码: 脚本与任务会注入 `LANG` / `LC_ALL` / `MSYS_UTF8` 环境变量以优先使用 UTF-8 输出；项目还包含 `platform_console.c/h` 模块，用于在运行时根据 `CONSOLE_ENCODING` 环境变量选择 UTF-8 或 GBK（Windows 下使用 SetConsoleCP/OutputCP）。

如果你希望我把这部分也同步到 `SDL_SETUP.md` 或把改动提交到一个分支，请回复确认。
This our c language group collaborative work repository.
Do not change the main branch over the captain`s permission.//We just want to make it smoothly to go on.

//everyone is suppoesed to follow the main branch`templatation