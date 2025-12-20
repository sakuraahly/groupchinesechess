//注意,到时候估计打包时外部链接资源(图片,音乐等)的路径又要改,先插个眼.
//要把这个文件和res文件放到同一个目录下 --luo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_main.h> //总之在目前的开发中不需要就是了
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

// ==================== 包含数据库头文件 ====================
#include "displayinterface.h"
#include "chess_database.h"

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

// 加载纹理
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





// 绘制选中指示器
void drawSelectedIndicator(SDL_Renderer* renderer) {
    if (is_piece_selected) {
        int screen_x = GRID_ORIGIN_X + selected_y * GRID_WIDTH - PIECE_SIZE/2;
        int screen_y = GRID_ORIGIN_Y + selected_x * GRID_HEIGHT - PIECE_SIZE/2;
        
        // 绘制黄色边框表示选中
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_Rect rect = {screen_x, screen_y, PIECE_SIZE, PIECE_SIZE};
        SDL_RenderDrawRect(renderer, &rect);
        
        // 绘制更粗的边框
        SDL_RenderDrawRect(renderer, &(SDL_Rect){screen_x+1, screen_y+1, PIECE_SIZE-2, PIECE_SIZE-2});
        SDL_RenderDrawRect(renderer, &(SDL_Rect){screen_x+2, screen_y+2, PIECE_SIZE-4, PIECE_SIZE-4});
    }
}

// 绘制当前玩家指示器
void drawCurrentPlayerIndicator(SDL_Renderer* renderer) {
    // 在屏幕右上角显示当前回合
    SDL_Color text_color = is_red_turn ? (SDL_Color){255, 50, 50, 255} : (SDL_Color){0, 0, 0, 255};
    const char* text = is_red_turn ? "红方回合" : "黑方回合";
    
    // 这里可以添加TTF字体渲染，为了简化，先输出到控制台
    printf("%s\n", text);
}

// 绘制游戏信息
void drawGameInfo(SDL_Renderer* renderer) {
    // 可以在这里绘制步数、思考时间等信息
    // 暂时只输出到控制台
    printf("当前步数: %d, 总步数: %d\n", move_step, current_game.move_count);
}

