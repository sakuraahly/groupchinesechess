
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

// 屏幕尺寸
#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 800

// 棋盘实际图片尺寸
#define BOARD_IMG_WIDTH  661
#define BOARD_IMG_HEIGHT 728

// 棋盘在屏幕上的显示尺寸
#define BOARD_DISPLAY_WIDTH  750
#define BOARD_DISPLAY_HEIGHT 820

// 棋子尺寸
#define PIECE_SIZE 60

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

// 棋盘上每个交叉点的坐标（基于661x728棋盘图片）
typedef struct {
    int x, y;
} BoardPosition;

// 中国象棋棋盘交叉点位置（基于标准棋盘布局）
BoardPosition boardPositions[10][9] = {
    // 第0行（黑方底线）
    {{56, 52}, {129, 52}, {202, 52}, {275, 52}, {348, 52}, {421, 52}, {494, 52}, {567, 52}, {604, 52}},
    // 第1行
    {{56, 125}, {129, 125}, {202, 125}, {275, 125}, {348, 125}, {421, 125}, {494, 125}, {567, 125}, {604, 125}},
    // 第2行
    {{56, 198}, {129, 198}, {202, 198}, {275, 198}, {348, 198}, {421, 198}, {494, 198}, {567, 198}, {604, 198}},
    // 第3行
    {{56, 271}, {129, 271}, {202, 271}, {275, 271}, {348, 271}, {421, 271}, {494, 271}, {567, 271}, {604, 271}},
    // 第4行
    {{56, 344}, {129, 344}, {202, 344}, {275, 344}, {348, 344}, {421, 344}, {494, 344}, {567, 344}, {604, 344}},
    // 第5行（楚河汉界）
    {{56, 417}, {129, 417}, {202, 417}, {275, 417}, {348, 417}, {421, 417}, {494, 417}, {567, 417}, {604, 417}},
    // 第6行
    {{56, 490}, {129, 490}, {202, 490}, {275, 490}, {348, 490}, {421, 490}, {494, 490}, {567, 490}, {604, 490}},
    // 第7行
    {{56, 563}, {129, 563}, {202, 563}, {275, 563}, {348, 563}, {421, 563}, {494, 563}, {567, 563}, {604, 563}},
    // 第8行
    {{56, 636}, {129, 636}, {202, 636}, {275, 636}, {348, 636}, {421, 636}, {494, 636}, {567, 636}, {604, 636}},
    // 第9行（红方底线）
    {{56, 676}, {129, 676}, {202, 676}, {275, 676}, {348, 676}, {421, 676}, {494, 676}, {567, 676}, {604, 676}}
};

// 棋子图片路径
const char* piece_names[] = {
    "none",
    "red_shuai.png", "red_ju.png", "red_ma.png", "red_pao.png", "red_shi.png", "red_xiang.png", "red_bing.png",
    "black_jiang.png", "black_ju.png", "black_ma.png", "black_pao.png", "black_shi.png", "black_xiang.png", "black_zu.png"
};

// 加载纹理
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path) {
    printf("尝试加载图片: %s\n", path);
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
        start_button = loadTexture(renderer, "res/images/button.png");
    }
    if (!start_button) {
        printf("无法加载按钮图片，将使用默认按钮\n");
    }

    // 加载棋子纹理
    SDL_Texture* pieces[15] = {NULL};
    printf("开始加载棋子纹理...\n");
    for (int i = 1; i < 15; i++) {
        char path[256];
        snprintf(path, sizeof(path), "res/images/%s", piece_names[i]);
        pieces[i] = loadTexture(renderer, path);
        
        if (!pieces[i]) {
            printf("!! 失败加载棋子 %d: %s\n", i, piece_names[i]);
        } else {
            printf("成功加载棋子 %d: %s\n", i, piece_names[i]);
        }
    }

    // 创建开始按钮
    SDL_Rect startButtonRect = {
        (SCREEN_WIDTH - 200) / 2,
        (SCREEN_HEIGHT - 80) / 2 + 100,
        200, 80
    };

    // 游戏状态
    GameState currentState = MENU_STATE;
    
    // 棋盘在屏幕上的位置（居中显示）
    int board_x = (SCREEN_WIDTH - BOARD_DISPLAY_WIDTH) / 2;
    int board_y = (SCREEN_HEIGHT - BOARD_DISPLAY_HEIGHT) / 2;

    printf("游戏初始化完成，当前状态: %s\n", currentState == MENU_STATE ? "菜单" : "游戏");
    printf("棋盘位置: x=%d, y=%d, 尺寸: %dx%d\n", board_x, board_y, BOARD_DISPLAY_WIDTH, BOARD_DISPLAY_HEIGHT);

    // 计算缩放比例
    float scale_x = (float)BOARD_DISPLAY_WIDTH / BOARD_IMG_WIDTH;
    float scale_y = (float)BOARD_DISPLAY_HEIGHT / BOARD_IMG_HEIGHT;
    printf("棋盘缩放比例: x=%.2f, y=%.2f\n", scale_x, scale_y);

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
                SDL_SetRenderDrawColor(renderer, 50, 50, 100, 255);
                SDL_RenderClear(renderer);
            }
            
            if (start_button) {
                SDL_RenderCopy(renderer, start_button, NULL, &startButtonRect);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 150, 200, 255);
                SDL_RenderFillRect(renderer, &startButtonRect);
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_RenderDrawRect(renderer, &startButtonRect);
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
                SDL_Rect boardRect = { board_x, board_y, BOARD_DISPLAY_WIDTH, BOARD_DISPLAY_HEIGHT };
                SDL_RenderCopy(renderer, chess_board, NULL, &boardRect);
            } else {
                // 如果没有棋盘图片，绘制简单棋盘
                SDL_SetRenderDrawColor(renderer, 210, 180, 140, 255);
                SDL_Rect boardRect = { board_x, board_y, BOARD_DISPLAY_WIDTH, BOARD_DISPLAY_HEIGHT };
                SDL_RenderFillRect(renderer, &boardRect);
            }
            
            // 绘制棋子 - 使用精确的位置计算
            printf("绘制棋子...\n");
            int pieceCount = 0;
            for (int row = 0; row < 10; row++) {
                for (int col = 0; col < 9; col++) {
                    PieceType piece = board[row][col];
                    if (piece != NONE) {
                        pieceCount++;
                        
                        if (pieces[piece]) {
                            // 获取棋盘交叉点在原始图片中的位置
                            BoardPosition pos = boardPositions[row][col];
                            
                            // 计算在屏幕上的位置（考虑缩放）
                            int screen_x = board_x + (int)(pos.x * scale_x) - PIECE_SIZE/2;
                            int screen_y = board_y + (int)(pos.y * scale_y) - PIECE_SIZE/2;
                            
                            SDL_Rect dest = {screen_x, screen_y, PIECE_SIZE, PIECE_SIZE};
                            SDL_RenderCopy(renderer, pieces[piece], NULL, &dest);
                            
                            printf("绘制棋子: type=%d, 棋盘位置=(%d,%d), 屏幕位置=(%d,%d)\n", 
                                   piece, row, col, screen_x, screen_y);
                        } else {
                            printf("!! 棋子纹理为空: type=%d, 位置=(%d,%d)\n", piece, row, col);
                            // 绘制占位矩形
                            BoardPosition pos = boardPositions[row][col];
                            int screen_x = board_x + (int)(pos.x * scale_x) - PIECE_SIZE/2;
                            int screen_y = board_y + (int)(pos.y * scale_y) - PIECE_SIZE/2;
                            SDL_Rect dest = {screen_x, screen_y, PIECE_SIZE, PIECE_SIZE};
                            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                            SDL_RenderFillRect(renderer, &dest);
                        }
                    }
                }
            }
            printf("总共绘制了 %d 个棋子\n", pieceCount);
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