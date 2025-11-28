#include <SDL.h>
#include <stdio.h>  // 添加这个头文件

// 使用标准的 main 函数
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    
    printf("SDL2 initialized successfully!\n");
    
    SDL_Quit();
    return 0;
}