//注意,到时候估计打包时外部链接资源(图片,音乐等)的路径又要改,先插个眼.
//要把这个文件和res文件放到同一个目录下 --luo

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>
#include <SDL2/SDL.h>
//#include <SDL_main.h> //总之在目前的开发中不需要就是了
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

// 屏幕尺寸[由于不确定究竟是怎么样的,这里存疑]
#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 800

// 棋盘在屏幕上的显示尺寸[没搞懂这个和上面的规则有什么关系]
#define BOARD_DISPLAY_WIDTH  750
#define BOARD_DISPLAY_HEIGHT 820

// 棋子尺寸[图片文件是60*60]
#define PIECE_SIZE 60

// ====== 分离的坐标系 ======
// 棋盘图片显示位置（视觉位置）
#define BOARD_VISUAL_X 225
#define BOARD_VISUAL_Y 50

// 棋盘格点坐标系原点（逻辑位置）- 这是关键！
#define GRID_ORIGIN_X 314
#define GRID_ORIGIN_Y 120

// 网格间距
#define GRID_WIDTH 68
#define GRID_HEIGHT 69

// 游戏状态
typedef enum {
    MENU_STATE,
    GAME_STATE
} GameState;

// ====== 新的棋子编码定义 ======
#define COLOR_RED    1  // 红方颜色代码
#define COLOR_BLACK  2  // 黑方颜色代码

// 棋子类型定义
#define TYPE_JIANG   1  // 将/帅
#define TYPE_SHI     2  // 士/仕
#define TYPE_XIANG   3  // 象/相
#define TYPE_MA      4  // 马
#define TYPE_JU      5  // 车
#define TYPE_PAO     6  // 炮
#define TYPE_BING    7  // 兵/卒

// 红方棋子编码（颜色代码1 + 棋子类型）
#define RED_SHUAI    11  // 红帅
#define RED_SHI      12  // 红士
#define RED_XIANG    13  // 红相
#define RED_MA       14  // 红马
#define RED_JU       15  // 红车
#define RED_PAO      16  // 红炮
#define RED_BING     17  // 红兵

// 黑方棋子编码（颜色代码2 + 棋子类型）
#define BLACK_JIANG  21  // 黑将
#define BLACK_SHI    22  // 黑仕
#define BLACK_XIANG  23  // 黑象
#define BLACK_MA     24  // 黑马
#define BLACK_JU     25  // 黑车
#define BLACK_PAO    26  // 黑炮
#define BLACK_ZU     27  // 黑卒

#define NONE         0  // 无棋子

// 棋盘布局  我想这总不会再搞错了
// 我也觉得-lin
int board[10][9] = {
    {BLACK_JU, BLACK_MA, BLACK_XIANG, BLACK_SHI, BLACK_JIANG, BLACK_SHI, BLACK_XIANG, BLACK_MA, BLACK_JU},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {NONE, BLACK_PAO, NONE, NONE, NONE, NONE, NONE, BLACK_PAO, NONE},
    {BLACK_ZU, NONE, BLACK_ZU, NONE, BLACK_ZU, NONE, BLACK_ZU, NONE, BLACK_ZU},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {RED_BING, NONE, RED_BING, NONE, RED_BING, NONE, RED_BING, NONE, RED_BING},
    {NONE, RED_PAO, NONE, NONE, NONE, NONE, NONE, RED_PAO, NONE},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {RED_JU, RED_MA, RED_XIANG, RED_SHI, RED_SHUAI, RED_SHI, RED_XIANG, RED_MA, RED_JU}
};

// 棋子图片路径 (注意：索引需要对应新的编码)
const char* piece_names[28] = {
    "none", // 索引0（NONE）
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 索引1-10
    "red_shuai.png",    // 索引11（RED_SHUAI）
    "red_shi.png",      // 索引12（RED_SHI）
    "red_xiang.png",    // 索引13（RED_XIANG）
    "red_ma.png",       // 索引14（RED_MA）
    "red_ju.png",       // 索引15（RED_JU）
    "red_pao.png",      // 索引16（RED_PAO）
    "red_bing.png",     // 索引17（RED_BING）
    NULL, NULL, NULL,   // 索引18-20
    "black_jiang.png",  // 索引21（BLACK_JIANG）
    "black_shi.png",    // 索引22（BLACK_SHI）
    "black_xiang.png",  // 索引23（BLACK_XIANG）
    "black_ma.png",     // 索引24（BLACK_MA）
    "black_ju.png",     // 索引25（BLACK_JU）
    "black_pao.png",    // 索引26（BLACK_PAO）
    "black_zu.png"      // 索引27（BLACK_ZU）
};

// 加载纹理 [真是白写,毕竟我创建的GUI程序看不到这些]  //注意这些path是只要有图片文件的相对路径就行了,同时以"路径"的格式作为参数传入
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path) {
    //printf("尝试加载图片: %s\n", path);
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        //printf("无法加载图片: %s, 错误: %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        //printf("创建纹理失败: %s\n", path);
    } else {
        //printf("成功加载图片: %s\n", path);
    }
    return texture;
}

// 检查点是否在矩形内{这个功能先留着吧,之后会用到的?}
bool pointInRect(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
}

// 辅助函数：获取棋子颜色
int getPieceColor(int piece) {
    if (piece == NONE) return 0;
    return piece / 10;
}

// 辅助函数：获取棋子类型
int getPieceType(int piece) {
    if (piece == NONE) return 0;
    return piece % 10;
}


//咳咳,然后这里这部分,考虑之后改变到game.c中,因为这里的东西太多了,而且还没有完全搞懂,所以先放这里吧,后面再整理.
// 主函数
int main(int argc, char* argv[]) {//塞一个void试试?
    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        //printf("SDL初始化失败: %s\n", SDL_GetError());
        return -1;
    }

    // 初始化SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        //printf("SDL_image初始化失败: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    // 初始化SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        //printf("SDL_mixer初始化失败: %s\n", Mix_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // 创建窗口和渲染器  [你可以把文本内容换成任何你想换成的东西]  {这个centered和shown就别管了,这是库里东西,大概就是居中显示的意思}
    SDL_Window* window = SDL_CreateWindow("中国象棋", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {//强调一下,这个创建失败是出现一个空指针,NULL在bool上是false的等价.
        //printf("创建窗口失败: %s\n", SDL_GetError());
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        //printf("创建渲染器失败: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }



    // 加载背景音乐
    Mix_Music* bgm = Mix_LoadMUS("res/music/bgm.mp3");
    if (bgm) {
        Mix_PlayMusic(bgm, -1);
    }

    // 加载资源
    SDL_Texture* background = loadTexture(renderer, "res/images/background.png");
    SDL_Texture* chess_board = loadTexture(renderer, "res/images/chess_board.png");
    SDL_Texture* start_button = loadTexture(renderer, "res/images/start_button.png");

    // 加载棋子纹理（使用新编码索引）
    SDL_Texture* pieces[28] = {NULL};
    for (int i = 11; i <= 27; i++) {
        if (piece_names[i] != NULL) {  // 只加载有对应图片的编码
            char path[256];
            snprintf(path, sizeof(path), "../res/images/%s", piece_names[i]);
            pieces[i] = loadTexture(renderer, path);
        }
    }

    // ====== 新增：加载侧边按钮图标 ======
    SDL_Texture* return_button = loadTexture(renderer, "res/images/return_to_menu.jpg");
    SDL_Texture* revoke_button = loadTexture(renderer, "res/images/revoke_chess.png");

    // 创建开始按钮{这些参数是猜测出来的,请见谅}
    SDL_Rect startButtonRect = {
        (SCREEN_WIDTH - 200) / 2,
        (SCREEN_HEIGHT - 80) / 2 + 100,
        200, 80
    };

    // ====== 新增：创建侧边按钮位置 ======
    SDL_Rect returnButtonRect = {
        30,   // 距离左侧30像素
        100,  // 距离顶部100像素
        100,  // 宽度
        100   // 高度
    };

    SDL_Rect revokeButtonRect = {
        30,   // 和返回按钮在同一列
        250,  // 在返回按钮下方150像素（100+100+50间距）
        159,  // 宽度
        86    // 高度
    };

    // 游戏状态
    GameState currentState = MENU_STATE;
    
    // 显示坐标系信息
    printf("=== 坐标系设置 ===\n");
    printf("棋盘显示位置: (%d, %d)\n", BOARD_VISUAL_X, BOARD_VISUAL_Y);
    printf("棋盘格点原点: (%d, %d)\n", GRID_ORIGIN_X, GRID_ORIGIN_Y);
    printf("网格间距: %dx%d\n", GRID_WIDTH, GRID_HEIGHT);
    printf("棋子尺寸: %d\n", PIECE_SIZE);
    printf("\n关键棋子理论位置:\n");
    printf("黑车(0,0): 格点(%d,%d)\n", 
           GRID_ORIGIN_X + 0 * GRID_WIDTH, GRID_ORIGIN_Y + 0 * GRID_HEIGHT);
    printf("黑将(0,4): 格点(%d,%d)\n", 
           GRID_ORIGIN_X + 4 * GRID_WIDTH, GRID_ORIGIN_Y + 0 * GRID_HEIGHT);
    printf("红帅(9,4): 格点(%d,%d)\n", 
           GRID_ORIGIN_X + 4 * GRID_WIDTH, GRID_ORIGIN_Y + 9 * GRID_HEIGHT);

    // 主循环[这里是游戏的本体部分,我们需要在这里实现游戏逻辑] {但是还没有写好,先放着吧,后面再补充}
    bool running = true;
    while (running) {
        SDL_Event event; //这又是一个SDL的事件结构体,大概就是有类型和鼠标按键位置
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            // 鼠标点击事件
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
    
                // 菜单状态下点击开始按钮
                if (currentState == MENU_STATE && pointInRect(mouseX, mouseY, startButtonRect)) {
                    currentState = GAME_STATE;
                }

                // ====== 新增：游戏状态下点击侧边按钮 ======
                if (currentState == GAME_STATE) {
                    // 检查"回到菜单"按钮
                    if (pointInRect(mouseX, mouseY, returnButtonRect)) {
                        printf("回到菜单\n");
                     currentState = MENU_STATE;
                 }
        
                    // 检查"悔棋"按钮
                    if (pointInRect(mouseX, mouseY, revokeButtonRect)) {
                        printf("悔棋\n");
                        // 这里需要实现悔棋功能
                    }
                }
            }
            
            // 按ESC返回菜单{其实这个功能也没什么卵用,反正就是个单机游戏,算了,之后再加上一个返回菜单的图标吧}
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                if (currentState == GAME_STATE) {
                    currentState = MENU_STATE;
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
            
            // 渲染棋盘（使用棋盘显示位置）
            if (chess_board) {
                SDL_Rect boardRect = { 
                    BOARD_VISUAL_X, 
                    BOARD_VISUAL_Y,
                    BOARD_DISPLAY_WIDTH, 
                    BOARD_DISPLAY_HEIGHT 
                };
                SDL_RenderCopy(renderer, chess_board, NULL, &boardRect);
            } else {
                // 如果没有棋盘图片，绘制简单棋盘(总感觉这是特么多余的)
                SDL_SetRenderDrawColor(renderer, 210, 180, 140, 255);
                SDL_Rect boardRect = { 
                    BOARD_VISUAL_X, 
                    BOARD_VISUAL_Y, 
                    BOARD_DISPLAY_WIDTH, 
                    BOARD_DISPLAY_HEIGHT 
                };
                SDL_RenderFillRect(renderer, &boardRect);
            }
            
            // 绘制棋子 - 使用独立的格点坐标系
            int pieceCount = 0;
            for (int x = 0; x < 10; x++) {      // 行 (x: 0-9)
                for (int y = 0; y < 9; y++) {   // 列 (y: 0-8)
                    int piece = board[x][y];
                    if (piece != NONE) {
                        pieceCount++;
                        
                        if (piece >= 11 && piece <= 27 && pieces[piece]) {
                            // 使用格点坐标系计算棋子位置（与棋盘显示位置无关）
                            int screen_x = GRID_ORIGIN_X + y * GRID_WIDTH - PIECE_SIZE/2;
                            int screen_y = GRID_ORIGIN_Y + x * GRID_HEIGHT - PIECE_SIZE/2;
                            
                            SDL_Rect dest = {screen_x, screen_y, PIECE_SIZE, PIECE_SIZE};
                            SDL_RenderCopy(renderer, pieces[piece], NULL, &dest);
                            
                            // 调试输出关键棋子位置
                            if ((x == 0 && y == 0) || (x == 0 && y == 4) || 
                                (x == 9 && y == 4) || (x == 9 && y == 0)) {
                                printf("棋子(%d,%d): 屏幕(%d,%d), 类型=%d\n", 
                                       x, y, screen_x, screen_y, piece);
                            }
                        } else {
                            // 棋子纹理加载失败，绘制占位矩形
                            int screen_x = GRID_ORIGIN_X + y * GRID_WIDTH - PIECE_SIZE/2;
                            int screen_y = GRID_ORIGIN_Y + x * GRID_HEIGHT - PIECE_SIZE/2;
                            SDL_Rect dest = {screen_x, screen_y, PIECE_SIZE, PIECE_SIZE};
                            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                            SDL_RenderFillRect(renderer, &dest);
                        }
                    }
                }
            }
            //printf("总共绘制了 %d 个棋子\n", pieceCount);

            // =============== 侧边按钮渲染代码 ===============
            // 渲染"回到菜单"按钮
            if (return_button) {
                SDL_RenderCopy(renderer, return_button, NULL, &returnButtonRect);
            } else {
                // 如果图标加载失败，绘制默认按钮
                SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
                SDL_RenderFillRect(renderer, &returnButtonRect);
                // 绘制边框
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &returnButtonRect);
            }
            
            // 渲染"悔棋"按钮
            if (revoke_button) {
                SDL_RenderCopy(renderer, revoke_button, NULL, &revokeButtonRect);
            } else {
                // 如果图标加载失败，绘制默认按钮
                SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255);
                SDL_RenderFillRect(renderer, &revokeButtonRect);
                // 绘制边框
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &revokeButtonRect);
            }
            // =============== 插入结束 ===============
        }

        // 呈现画面
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // 清理资源,这些都没有什么好讲的,总之就是清理我创建的窗口了{不然你就要手动杀进程//否则内存泄露//大家玩完}
    //printf("清理资源...\n");

    if (bgm) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
    
    for (int i = 0; i < 28; i++) {
        if (pieces[i]) SDL_DestroyTexture(pieces[i]);
    }
    if (background) SDL_DestroyTexture(background);
    if (chess_board) SDL_DestroyTexture(chess_board);
    if (start_button) SDL_DestroyTexture(start_button);

    // ====== 新增：清理侧边按钮纹理 ======
    if (return_button) SDL_DestroyTexture(return_button);
    if (revoke_button) SDL_DestroyTexture(revoke_button);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    printf("游戏退出\n");
    return 0;
}