#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    printf("Program starting...\n");
    fflush(stdout);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        fflush(stdout);
        return 1;
    }
    printf("SDL initialized successfully.\n");
    fflush(stdout);

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        printf("IMG_Init error: %s\n", IMG_GetError());
        fflush(stdout);
        SDL_Quit();
        return 1;
    }
    printf("SDL_image initialized successfully.\n");
    fflush(stdout);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mix_OpenAudio error: %s\n", Mix_GetError());
        fflush(stdout);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    printf("SDL_mixer initialized successfully.\n");
    fflush(stdout);
    
    // 尝试创建窗口
    printf("Attempting to create window...\n");
    fflush(stdout);
    
    SDL_Window *window = SDL_CreateWindow("SDL Test", 
                                         SDL_WINDOWPOS_CENTERED, 
                                         SDL_WINDOWPOS_CENTERED, 
                                         800, 600, 
                                         SDL_WINDOW_SHOWN);
    
    if (window == NULL) {
        printf("CRITICAL: Failed to create window! Error: %s\n", SDL_GetError());
        fflush(stdout);
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    printf("Window created successfully!\n");
    fflush(stdout);
    
    // 尝试创建渲染器
    printf("Attempting to create renderer...\n");
    fflush(stdout);
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
                                               SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (renderer == NULL) {
        printf("CRITICAL: Failed to create renderer! Error: %s\n", SDL_GetError());
        fflush(stdout);
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    printf("Renderer created successfully!\n");
    fflush(stdout);
    
    // 设置渲染颜色并清空屏幕
    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
    SDL_RenderClear(renderer);
    
    // 绘制一个红色矩形
    SDL_Rect rect = {300, 200, 200, 200};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    
    // 更新屏幕
    SDL_RenderPresent(renderer);
    printf("Graphics rendered successfully!\n");
    fflush(stdout);
    
    printf("Entering main event loop. Window should be visible now.\n");
    printf("Press any key, click mouse, or close window to exit.\n");
    fflush(stdout);
    
    // 事件循环
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                printf("SDL_QUIT event received.\n");
                fflush(stdout);
                running = 0;
            }
            if (event.type == SDL_KEYDOWN) {
                printf("Key pressed: %d\n", event.key.keysym.sym);
                fflush(stdout);
                running = 0;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                printf("Mouse button pressed at (%d, %d)\n", event.button.x, event.button.y);
                fflush(stdout);
                running = 0;
            }
        }
        SDL_Delay(16);
    }
    
    // 清理资源
    printf("Cleaning up resources...\n");
    fflush(stdout);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
    
    printf("Program exited successfully.\n");
    fflush(stdout);
    return 0;
}