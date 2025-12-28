#编译与运行

如果想要编译并运行这个该死的东西,请在终端输入这个命令(来自captain的电脑的实验)
//由于本地库的问题,建议在安装好依赖之后再操作.
//总之推荐使用msys2的mingw64环境来编译运行这个项目.
//这个也可以很方便的安装其他的包

//注意: 请根据你的SDL2安装路径修改下面的命令中的路径部分.
gcc -IC:/msys64/mingw64/include/SDL2 -LC:/msys64/mingw64/lib -o ./output/game.exe ./src/game.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -mwindows
//I 是指定头文件的搜索路径, L 是指定库文件的搜索路径, -o 是指定输出文件名.
//这个命令输出的是 game.exe 这个文件输出到output文件夹

新的编译命令(用于displayinterface.c)
//gcc -IC:/msys64/mingw64/include/SDL2 -I./src -LC:/msys64/mingw64/lib -o ./output/game.exe ./src/chess_database.c ./src/displayinterface.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -mwindows

用于游戏文件game.c的编译命令:
//gcc -IC:/msys64/mingw64/include/SDL2 -I./src/include -LC:/msys64/mingw64/lib -o ./output/game.exe ./src/game.c ./src/displayinterface.c  ./src/chess_database.c ./src/chess_move.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -mwindows
