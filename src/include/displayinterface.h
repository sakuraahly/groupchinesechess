// 注意,到时候估计打包时外部链接资源(图片,音乐等)的路径又要改,先插个眼.
// 要把这个文件和res文件放到同一个目录下 --luo
#ifndef DISPLAYINTERFACE_H
#define DISPLAYINTERFACE_H

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

// 包含自己的头文件
#include "chess_database.h"
// 这些是渲染器,图片资源等 -hu 12.28
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *background;
extern SDL_Texture *chess_board;
extern SDL_Texture *start_button;
extern SDL_Texture *special_mode;
// ====== 新增：加载侧边按钮图标 ====== 这些是游戏界面的侧边按钮
extern SDL_Texture *return_button;
extern SDL_Texture *revoke_button;
extern SDL_Texture *save_button;
// 新增"撤销悔棋"按钮
extern SDL_Texture *redo_button;
// 新增：加载"继续游戏"按钮图片（用于菜单界面）
extern SDL_Texture *continue_button;
// 新加载胜利图片 -hu 12.26
extern SDL_Texture *red_victory_image;
extern SDL_Texture *black_victory_image;
// 新增继续游戏图片 -hu 12.26
extern SDL_Texture *continue_image;
// 加载投降按钮 -hu 12.28
extern SDL_Texture *surrender_button;

// 图片位置结构体定义
extern SDL_Rect startButtonRect;
extern SDL_Rect continueButtonRect;
extern SDL_Rect returnButtonRect;
extern SDL_Rect revokeButtonRect;
extern SDL_Rect saveButtonRect;
extern SDL_Rect redoButtonRect;
extern SDL_Rect redVictoryRect;
extern SDL_Rect blackVictoryImageRect;
extern SDL_Rect surrenderButtonRect;
extern SDL_Rect specialModeRect;
extern SDL_Texture *pieces[28];

// 外部变量的定义
extern const char *piece_names[28]; // 棋子图片路径数组
extern int board[10][9];            // 棋盘数组
// 游戏胜利标志
extern bool redFlyToWin;   // 红方飞将胜利标志
extern bool blackFlyToWin; // 黑方飞将胜利标志
extern bool is_shuai_live; // 红帅是否存活
extern bool is_jiang_live; // 黑将是否存活

extern int board[10][9];

extern bool is_music_playing; // 背景音乐的判定
extern Mix_Music *bgm;
extern Mix_Chunk *choseChess; // 捕获音效
extern Mix_Chunk *eat;        // 吃子音效
extern Mix_Chunk *jiangjun;
extern Mix_Chunk *win;       // 胜利音效
extern Mix_Chunk *surrender; // 失败音效
// extern Mix_Music* chess_move_sound; //下棋音效

// 屏幕尺寸[由于不确定究竟是怎么样的,这里存疑]
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

// 棋盘在屏幕上的显示尺寸[没搞懂这个和上面的规则有什么关系] -hu 12.05
#define BOARD_DISPLAY_WIDTH 750
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

// ====== 棋子编码定义（已在chess_database.h中定义） ======
#define NONE 0 // 无棋子

// 资源加载代码 -hu 12.28
void init_resources();

// 音频函数声明
void init_music();

// 函数声明
SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *path);
bool screenToBoard(int screen_x, int screen_y, int *board_x, int *board_y);
void drawSelectedIndicator(SDL_Renderer *renderer);
void drawCurrentPlayerIndicator(SDL_Renderer *renderer);
void drawGameInfo(SDL_Renderer *renderer);

// 把棋盘恢复定义的标准状态 -hu 12.27
void restoreBoardToStandardState();

// 清理资源函数 // -hu 12.28
void cleanup_resources();

#endif // DISPLAYINTERFACE_H