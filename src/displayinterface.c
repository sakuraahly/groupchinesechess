// 注意,到时候估计打包时外部链接资源(图片,音乐等)的路径又要改,先插个眼.
// 要把这个文件和res文件放到同一个目录下 --luo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <SDL2/SDL.h>
// #include <SDL2/SDL_main.h> //总之在目前的开发中不需要就是了
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

// 包含数据库头文件,这些都在sr/include目录下 -hu 12.08
#include "displayinterface.h"
#include "chess_database.h"
#include "chess_move.h"

// 加载图片资源的地方 -hu 12.28
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *background;
SDL_Texture *chess_board;
SDL_Texture *start_button;
SDL_Texture *special_mode;
SDL_Texture *return_button;
SDL_Texture *revoke_button;
SDL_Texture *save_button;
// 新增"撤销悔棋"按钮
SDL_Texture *redo_button;
// 新增：加载"继续游戏"按钮图片（用于菜单界面）
SDL_Texture *continue_button;
// 新加载胜利图片 -hu 12.26
SDL_Texture *red_victory_image;
SDL_Texture *black_victory_image;
// 新增继续游戏图片 -hu 12.26
SDL_Texture *continue_image;
// 加载投降按钮 -hu 12.28
SDL_Texture *surrender_button;
SDL_Texture *pieces[28] = {NULL};

// 加载音频文件 注意,这里是播放背景音乐,然后我准备用mixer修复音乐被打断的问题. -hu 12.15
// 顺便,这里也加载走子音频得了,在头文件加入了声明 -hu 12.21
Mix_Music *bgm;
Mix_Chunk *choseChess;
Mix_Chunk *eat;
Mix_Chunk *jiangjun;
Mix_Chunk *win;
Mix_Chunk *surrender;
bool is_music_playing;
void init_music()
{
    bgm = Mix_LoadMUS("res/music/bgm.mp3");
    choseChess = Mix_LoadWAV("res/music/chose.mp3");
    eat = Mix_LoadWAV("res/music/eat.mp3");
    is_music_playing = false;
    jiangjun = Mix_LoadWAV("res/music/jiang.wav");
    win = Mix_LoadWAV("res/music/win.wav");
    surrender = Mix_LoadWAV("res/music/surrender.wav");
}

// 棋盘布局  我想这总不会再搞错了-hu //现在修改为变量棋盘
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
    {RED_JU, RED_MA, RED_XIANG, RED_SHI, RED_SHUAI, RED_SHI, RED_XIANG, RED_MA, RED_JU}};

// 标准棋盘布局的备份
int standard_board[10][9] = {
    {BLACK_JU, BLACK_MA, BLACK_XIANG, BLACK_SHI, BLACK_JIANG, BLACK_SHI, BLACK_XIANG, BLACK_MA, BLACK_JU},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {NONE, BLACK_PAO, NONE, NONE, NONE, NONE, NONE, BLACK_PAO, NONE},
    {BLACK_ZU, NONE, BLACK_ZU, NONE, BLACK_ZU, NONE, BLACK_ZU, NONE, BLACK_ZU},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {RED_BING, NONE, RED_BING, NONE, RED_BING, NONE, RED_BING, NONE, RED_BING},
    {NONE, RED_PAO, NONE, NONE, NONE, NONE, NONE, RED_PAO, NONE},
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
    {RED_JU, RED_MA, RED_XIANG, RED_SHI, RED_SHUAI, RED_SHI, RED_XIANG, RED_MA, RED_JU}};

// 创建开始按钮
SDL_Rect startButtonRect = {
    (SCREEN_WIDTH - 200) / 2,
    (SCREEN_HEIGHT - 80) / 2 + 100,
    200, 80};

// "继续游戏"按钮的位置定义
SDL_Rect continueButtonRect = {
    (SCREEN_WIDTH - 200) / 2,
    (SCREEN_HEIGHT - 80) / 2 + 200,
    200, 80};

// 创建侧边按钮位置
SDL_Rect returnButtonRect = {
    30,  // 距离左侧30像素
    100, // 距离顶部100像素
    100, // 宽度
    100  // 高度
};

SDL_Rect revokeButtonRect = {
    30,  // 和返回按钮在同一列
    250, // 在返回按钮下方150像素（100+100+50间距）
    159, // 宽度
    86   // 高度
};

// 新增保存按钮位置
SDL_Rect saveButtonRect = {
    30,  // 和悔棋按钮在同一列
    400, // 在悔棋按钮下方
    100, // 宽度
    50   // 高度
};

// ====== 新增："撤销悔棋"按钮位置 ======
SDL_Rect redoButtonRect = {
    30,  // 和保存按钮在同一列
    480, // 在保存按钮下方（400+50+30间距）
    100, // 宽度
    50   // 高度
};

// 这是投降的按钮 -- hu 12.28
SDL_Rect surrenderButtonRect = {
    30,  // 和撤销悔棋按钮在同一列
    530, // 在撤销悔棋按钮下方（480+50+30间距）
    100, // 宽度
    50   // 高度
};

// 增加 胜利与失败的图片的位置. -hu 12.26
SDL_Rect redVictoryRect = {
    30,  // 距离左侧30像素
    560, // 距离顶部560像素
    200, // 宽度
    200  // 高度
};

SDL_Rect specialModeRect = {
    30,  // 距离左侧30像素
    560, // 距离顶部560像素
    200, // 宽度
    200  // 高度
};

// 将被特色模式修改过的布局恢复到全局数组的状态 -hu 12.27
// 由于全局数组的存储空间固定,这个样子使用不用担心值传递的副本问题,同样的,也可以这样修改全局变量.
void restoreBoardToStandardState()
{
    for (int x = 0; x < 10; x++)
    {
        {
            for (int y = 0; y < 9; y++)
            {
                board[x][y] = standard_board[x][y];
            }
        }
    }
}
// 棋子图片路径 (注意：索引需要对应新的编码)
const char *piece_names[28] = {
    "none",                                                     // 索引0（NONE）
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 索引1-10
    "red_shuai.png",                                            // 索引11（RED_SHUAI）
    "red_shi.png",                                              // 索引12（RED_SHI）
    "red_xiang.png",                                            // 索引13（RED_XIANG）
    "red_ma.png",                                               // 索引14（RED_MA）
    "red_ju.png",                                               // 索引15（RED_JU）
    "red_pao.png",                                              // 索引16（RED_PAO）
    "red_bing.png",                                             // 索引17（RED_BING）
    NULL, NULL, NULL,                                           // 索引18-20
    "black_jiang.png",                                          // 索引21（BLACK_JIANG）
    "black_shi.png",                                            // 索引22（BLACK_SHI）
    "black_xiang.png",                                          // 索引23（BLACK_XIANG）
    "black_ma.png",                                             // 索引24（BLACK_MA）
    "black_ju.png",                                             // 索引25（BLACK_JU）
    "black_pao.png",                                            // 索引26（BLACK_PAO）
    "black_zu.png"                                              // 索引27（BLACK_ZU）
};

// 加载图片的函数,在game.c中直接用就是了,一般不会有问题 -hu 11.18
SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    if (!surface)
    {
        // printf("无法加载图片: %s, 错误: %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

// 尝试重构代码 -hu 12.28 简化运行逻辑
void init_resources()
{
    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        // printf("SDL初始化失败: %s\n", SDL_GetError());
        // return -1;
    }

    // 初始化SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        // printf("SDL_image初始化失败: %s\n", IMG_GetError());
        SDL_Quit();
        // return -1;
    }

    // 初始化SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        // printf("SDL_mixer初始化失败: %s\n", Mix_GetError());
        IMG_Quit();
        SDL_Quit();
        // return -1;
    }
    // 创建窗口和渲染器(可以改变显示框顶部的标题)
    window = SDL_CreateWindow("中国象棋", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    { // 强调一下,这个创建失败是出现一个空指针,NULL在bool上是false的等价.
        // printf("创建窗口失败: %s\n", SDL_GetError());
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        // return -1;
    }
    // 创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        // printf("创建渲染器失败: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        // return -1;
    }
    // 加载图片

    // 加载资源(这些是菜单和背景)
    background = loadTexture(renderer, "res/images/background.png");
    chess_board = loadTexture(renderer, "res/images/chess_board.png");
    start_button = loadTexture(renderer, "res/images/start_button.png");
    special_mode = loadTexture(renderer, "res/images/special_mode.png");
    // ====== 新增：加载侧边按钮图标 ====== 这些是游戏界面的侧边按钮
    return_button = loadTexture(renderer, "res/images/return_to_menu.png");
    revoke_button = loadTexture(renderer, "res/images/revoke_chess.png");
    // 新增保存按钮
    save_button = loadTexture(renderer, "res/images/save_button.png");
    // 新增"撤销悔棋"按钮
    redo_button = loadTexture(renderer, "res/images/redo_button.png");

    // 新增：加载"继续游戏"按钮图片（用于菜单界面）
    continue_button = loadTexture(renderer, "res/images/continue_button.png");

    // 新加载胜利图片 -hu 12.26
    red_victory_image = loadTexture(renderer, "res/images/red_win.png");
    black_victory_image = loadTexture(renderer, "res/images/black_win.png");

    // 新增继续游戏图片 -hu 12.26
    continue_image = loadTexture(renderer, "res/images/continue_image.png");

    // 加载投降按钮 -hu 12.28
    surrender_button = loadTexture(renderer, "res/images/surrender_image.png");
    // 加载棋子图片
    for (int i = 11; i <= 27; i++)
    {
        if (piece_names[i] != NULL)
        { // 只加载有对应图片的编码
            char path[256];
            snprintf(path, sizeof(path), "./res/images/%s", piece_names[i]);
            pieces[i] = loadTexture(renderer, path);
        }
    }
}

// 绘制选中指示器{就是黄色的方框}
void drawSelectedIndicator(SDL_Renderer *renderer)
{
    if (is_piece_selected)
    {
        int screen_x = GRID_ORIGIN_X + selected_y * GRID_WIDTH - PIECE_SIZE / 2;
        int screen_y = GRID_ORIGIN_Y + selected_x * GRID_HEIGHT - PIECE_SIZE / 2;

        // 绘制黄色边框表示选中 这里还有只能再次点击黄色方框取消的问题 -hu 12.21
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_Rect rect = {screen_x, screen_y, PIECE_SIZE, PIECE_SIZE};
        SDL_RenderDrawRect(renderer, &rect);

        // 绘制更粗的边框
        SDL_RenderDrawRect(renderer, &(SDL_Rect){screen_x + 1, screen_y + 1, PIECE_SIZE - 2, PIECE_SIZE - 2});
        SDL_RenderDrawRect(renderer, &(SDL_Rect){screen_x + 2, screen_y + 2, PIECE_SIZE - 4, PIECE_SIZE - 4});

        //-1表示自动选择可用的频道,然后0是播放一次的意思
        // 不行,不能在这里,事件处理循环会让音乐循环播放. -hu 12.21
        // Mix_PlayChannel(-1, choseChess, 0);
        // Mix_PlayMusic(choseChess, 0);
    }
}
