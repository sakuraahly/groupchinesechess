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

// 加载音频文件 注意,这里是播放背景音乐,然后我准备用mixer修复音乐被打断的问题. -hu 12.15
// 顺便,这里也加载走子音频得了,在头文件加入了extern声明 -hu 12.21
Mix_Music *bgm;
Mix_Chunk *choseChess;
Mix_Chunk *eat;
Mix_Chunk *jiangjun;
Mix_Chunk *win;
bool is_music_playing;
void init_music()
{
    bgm = Mix_LoadMUS("res/music/bgm.mp3");
    choseChess = Mix_LoadWAV("res/music/chose.mp3");
    eat = Mix_LoadWAV("res/music/eat.mp3");
    is_music_playing = false;
    jiangjun = Mix_LoadWAV("res/music/jiang.wav");
    win = Mix_LoadWAV("res/music/win.wav");
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
