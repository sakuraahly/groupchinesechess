
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <SDL2/SDL.h>
//#include <SDL_main.h> //总之在目前的开发中不需要就是了
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>


// 屏幕尺寸
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define BOARD_SIZE    540
#define CELL_SIZE     60
#define PIECE_SIZE    50

// 游戏状态
typedef enum {
    MENU_STATE,
    GAME_STATE
} GameState;

// 棋子类型
typedef enum {
    NONE = 0,
    RED_SHUAI, RED_JU, RED_MA, RED_PAO, RED_SHI, RED_XIANG, RED_BING,
    BLACK_JIANG, BLACK_JU, BLACK_MA, BLACK_PAO, BLACK_SHI, BLACK_XIANG, BLACK_ZU
} PieceType;

// 棋盘布局
PieceType board[10][9] = {
    {BLACK_JU, BLACK_MA, BLACK_XIANG, BLACK_SHI, BLACK_JIANG, BLACK_SHI, BLACK_XIANG, BLACK_MA, BLACK_JU},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {NONE, BLACK_PAO, NONE, NONE, NONE, NONE, NONE, BLACK_PAO, NONE},
    {BLACK_ZU, NONE, BLACK_ZU, NONE, BLACK_ZU, NONE, BLACK_ZU, NONE, BLACK_ZU},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {RED_BING, NONE, RED_BING, NONE, RED_BING, NONE, RED_BING, NONE, RED_BING},
    {NONE, RED_PAO, NONE, NONE, NONE, NONE, NONE, RED_PAO, NONE},
    {RED_JU, RED_MA, RED_XIANG, RED_SHI, RED_SHUAI, RED_SHI, RED_XIANG, RED_MA, RED_JU},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE}
};

// 棋子图片路径
const char* piece_names[] = {
    "none",
    "red_shuai.png", "red_ju.png", "red_ma.png", "red_pao.png", "red_shi.png", "red_xiang.png", "red_bing.png",
    "black_jiang.png", "black_ju.png", "black_ma.png", "black_pao.png", "black_shi.png", "black_xiang.png", "black_zu.png"
};

// 加载纹理
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path) {
    printf("尝试加载图片: %s\n", path); // 添加调试信息
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("无法加载图片: %s, 错误: %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        printf("创建纹理失败: %s\n", path);
    } else {
        printf("成功加载图片: %s\n", path);
    }
    return texture;
}

// 检查点是否在矩形内
bool pointInRect(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
}

int main(int argc, char* argv[]) {
    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL初始化失败: %s\n", SDL_GetError());
        return -1;
    }

    // 初始化SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image初始化失败: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    // 初始化SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer初始化失败: %s\n", Mix_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // 创建窗口和渲染器
    SDL_Window* window = SDL_CreateWindow("中国象棋", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("创建窗口失败: %s\n", SDL_GetError());
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("创建渲染器失败: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // 加载背景音乐
    printf("尝试加载背景音乐...\n");
    Mix_Music* bgm = Mix_LoadMUS("res/music/bgm.MP3");
    if (!bgm) {
        printf("无法加载背景音乐: %s\n", Mix_GetError());
        printf("请确保文件 res/music/bgm.MP3 存在\n");
    } else {
        // 循环播放背景音乐
        Mix_PlayMusic(bgm, -1);
        printf("背景音乐开始播放\n");
    }

    // 加载资源
    printf("加载游戏资源...\n");
    SDL_Texture* background = loadTexture(renderer, "res/images/background.png");
    SDL_Texture* chess_board = loadTexture(renderer, "res/images/chess_board.png");
    
    // 尝试多个可能的按钮图片路径
    SDL_Texture* start_button = loadTexture(renderer, "res/images/start_button.png");
    if (!start_button) {
        // 尝试其他可能的文件名
        start_button = loadTexture(renderer, "res/images/button.png");
    }
    if (!start_button) {
        printf("无法加载按钮图片，将使用默认按钮\n");
    }

    // 加载棋子纹理
    SDL_Texture* pieces[15] = {NULL};
    for (int i = 1; i < 15; i++) {
        char path[256];
        snprintf(path, sizeof(path), "res/images/%s", piece_names[i]);
        pieces[i] = loadTexture(renderer, path);
    }

    // 创建开始按钮
    SDL_Rect startButtonRect = {
        (SCREEN_WIDTH - 200) / 2,
        (SCREEN_HEIGHT - 80) / 2 + 100,
        200, 80
    };

    // 游戏状态
    GameState currentState = MENU_STATE;
    int board_x = (SCREEN_WIDTH - BOARD_SIZE) / 2;
    int board_y = (SCREEN_HEIGHT - BOARD_SIZE) / 2;

    printf("游戏初始化完成，当前状态: %s\n", currentState == MENU_STATE ? "菜单" : "游戏");
    printf("按钮位置: x=%d, y=%d, w=%d, h=%d\n", 
           startButtonRect.x, startButtonRect.y, startButtonRect.w, startButtonRect.h);

    // 主循环
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            // 鼠标点击事件
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                printf("鼠标点击: (%d, %d)\n", mouseX, mouseY);
                
                // 菜单状态下点击开始按钮
                if (currentState == MENU_STATE && pointInRect(mouseX, mouseY, startButtonRect)) {
                    currentState = GAME_STATE;
                    printf("开始对弈！切换到游戏界面\n");
                } else if (currentState == MENU_STATE) {
                    printf("点击位置不在按钮范围内\n");
                }
            }
            
            // 按ESC返回菜单
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                if (currentState == GAME_STATE) {
                    currentState = MENU_STATE;
                    printf("返回菜单\n");
                }
            }
        }

        // 清屏
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 渲染界面
        if (currentState == MENU_STATE) {
            // 渲染菜单界面
            if (background) {
                SDL_RenderCopy(renderer, background, NULL, NULL);
            } else {
                // 如果没有背景图，使用纯色背景
                SDL_SetRenderDrawColor(renderer, 50, 50, 100, 255);
                SDL_RenderClear(renderer);
            }
            
            if (start_button) {
                SDL_RenderCopy(renderer, start_button, NULL, &startButtonRect);
            } else {
                // 绘制更明显的默认按钮
                SDL_SetRenderDrawColor(renderer, 0, 150, 200, 255);
                SDL_RenderFillRect(renderer, &startButtonRect);
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_RenderDrawRect(renderer, &startButtonRect);
                
                // 在按钮上绘制文字（简单模拟）
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_Rect textRect = {startButtonRect.x + 50, startButtonRect.y + 30, 100, 20};
                SDL_RenderFillRect(renderer, &textRect);
            }
        } else {
            // 渲染游戏界面
            if (background) {
                SDL_RenderCopy(renderer, background, NULL, NULL);
            } else {
                SDL_SetRenderDrawColor(renderer, 50, 50, 100, 255);
                SDL_RenderClear(renderer);
            }
            
            if (chess_board) {
                SDL_Rect boardRect = { board_x, board_y, BOARD_SIZE, BOARD_SIZE };
                SDL_RenderCopy(renderer, chess_board, NULL, &boardRect);
            } else {
                // 如果没有棋盘图，绘制简单棋盘
                SDL_SetRenderDrawColor(renderer, 210, 180, 140, 255);
                SDL_Rect boardRect = { board_x, board_y, BOARD_SIZE, BOARD_SIZE };
                SDL_RenderFillRect(renderer, &boardRect);
            }
            
            // 绘制棋子
            for (int row = 0; row < 10; row++) {
                for (int col = 0; col < 9; col++) {
                    PieceType piece = board[row][col];
                    if (piece != NONE && pieces[piece]) {
                        int x = board_x + col * CELL_SIZE + (CELL_SIZE - PIECE_SIZE) / 2;
                        int y = board_y + row * CELL_SIZE + (CELL_SIZE - PIECE_SIZE) / 2;
                        SDL_Rect dest = {x, y, PIECE_SIZE, PIECE_SIZE};
                        SDL_RenderCopy(renderer, pieces[piece], NULL, &dest);
                    }
                }
            }
        }

        // 呈现画面
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // 清理资源
    printf("清理资源...\n");
    if (bgm) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
    
    for (int i = 0; i < 15; i++) {
        if (pieces[i]) SDL_DestroyTexture(pieces[i]);
    }
    if (background) SDL_DestroyTexture(background);
    if (chess_board) SDL_DestroyTexture(chess_board);
    if (start_button) SDL_DestroyTexture(start_button);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    printf("游戏退出\n");
    return 0;
}