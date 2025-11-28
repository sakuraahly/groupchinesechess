
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <SDL.h>
//#include <SDL_main.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>



// 定义棋盘大小和屏幕尺寸
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define BOARD_SIZE    540  // 棋盘宽度
#define CELL_SIZE     60   // 每格大小（实际根据棋盘调整）

// 棋子类型枚举
typedef enum {
    NONE,
    RED_SHUAI, RED_JU, RED_MA, RED_PAO, RED_SHI, RED_XIANG, RED_BING,
    BLACK_JIANG, BLACK_JU, BLACK_MA, BLACK_PAO, BLACK_SHI, BLACK_XIANG, BLACK_ZU
} PieceType;

// 棋盘布局（初始状态）
PieceType board[10][9] = {
    {BLACK_JU, BLACK_MA, BLACK_XIANG, BLACK_SHI, BLACK_JIANG, BLACK_SHI, BLACK_XIANG, BLACK_MA, BLACK_JU},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {NONE, BLACK_PAO, NONE, NONE, NONE, NONE, NONE, BLACK_PAO, NONE},
    {NONE, BLACK_ZU, NONE, BLACK_ZU, NONE, BLACK_ZU, NONE, BLACK_ZU, NONE},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {NONE, RED_BING, NONE, RED_BING, NONE, RED_BING, NONE, RED_BING, NONE},
    {NONE, RED_PAO, NONE, NONE, NONE, NONE, NONE, RED_PAO, NONE},
    {RED_JU, RED_MA, RED_XIANG, RED_SHI, RED_SHUAI, RED_SHI, RED_XIANG, RED_MA, RED_JU},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE}
};

// 图片路径映射表
const char* piece_names[] = {
    "none",
    "red-shuai.png", "red-ju.png", "red-ma.png", "red-pao.png", "red-shi.png", "red-xiang.png", "red-bing.png",
    "black-jiang.png", "black-ju.png", "black-ma.png", "black-pao.png", "black-shi.png", "black-xiang.png", "black-zu.png"
};

// 加载纹理函数
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("无法加载图片: %s\n", path);
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        printf("创建纹理失败: %s\n", path);
    }

    return texture;
}

int main(int argc, char* argv[]) {
    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL初始化失败: %s\n", SDL_GetError());
        return -1;
    }

    // 创建窗口
    SDL_Window* window = SDL_CreateWindow("中国象棋", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("创建窗口失败: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // 创建渲染器
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("创建渲染器失败: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 初始化 SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image初始化失败: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 加载所有资源
    SDL_Texture* background = loadTexture(renderer, "res/images/background.png");
    SDL_Texture* chess_board = loadTexture(renderer, "res/imageschess_board.png");
    SDL_Texture* frame = loadTexture(renderer, "res/imagesframe.png");

    // 加载棋子纹理
SDL_Texture* pieces[17];  // 最多17种棋子
for (int i = 0; i < 17; ++i) {
    char path[256];
    snprintf(path, sizeof(path), "assets/%s", i == 0 ? "none" : piece_names[i]);
    pieces[i] = loadTexture(renderer, path);
}

    // 渲染循环
    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // 清屏
        SDL_RenderClear(renderer);

        // 绘制背景
        SDL_RenderCopy(renderer, background, NULL, NULL);

        // 绘制棋盘
        SDL_Rect boardRect = { (SCREEN_WIDTH - BOARD_SIZE) / 2, (SCREEN_HEIGHT - BOARD_SIZE) / 2, BOARD_SIZE, BOARD_SIZE };
        SDL_RenderCopy(renderer, chess_board, NULL, &boardRect);

        // 绘制棋子
        for (int row = 0; row < 10; ++row) {
            for (int col = 0; col < 9; ++col) {
                if (board[row][col] != NONE) {
                    int x = (SCREEN_WIDTH - BOARD_SIZE) / 2 + col * CELL_SIZE;
                    int y = (SCREEN_HEIGHT - BOARD_SIZE) / 2 + row * CELL_SIZE;

                    SDL_Rect dest = {x, y, CELL_SIZE, CELL_SIZE};
                    SDL_RenderCopy(renderer, pieces[board[row][col]], NULL, &dest);
                }
            }
        }

        // 呈现画面
        SDL_RenderPresent(renderer);
    }

    // 清理资源
    for (int i = 0; i < 17; ++i) {
        if (pieces[i]) SDL_DestroyTexture(pieces[i]);
    }
    if (background) SDL_DestroyTexture(background);
    if (chess_board) SDL_DestroyTexture(chess_board);
    if (frame) SDL_DestroyTexture(frame);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}