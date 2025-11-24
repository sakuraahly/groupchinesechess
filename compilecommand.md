如果想要编译并运行这个该死的东西,请在终端输入这个命令(来自captain的电脑的实验)
//由于本地库的问题,建议在安装好依赖之后再操作.
gcc -IC:/msys64/mingw64/include/SDL2 -LC:/msys64/mingw64/lib -o user.exe main_sdlc.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -mwindows