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
#include "chess_database.h"

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

// ====== 棋子编码定义（已在chess_database.h中定义） ======
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

// ==================== 新增全局变量 ====================
static GameRecord current_game;        // 当前棋局记录
static ChessMove current_move;         // 当前棋步记录
static bool is_red_turn = true;        // 当前轮到红方走棋
static int move_step = 1;              // 当前步数计数器
static time_t move_start_time = 0;     // 当前步开始时间

// 棋子选择状态
static bool is_piece_selected = false;
static int selected_x = -1;
static int selected_y = -1;
static int selected_piece = NONE;

// ==================== 函数声明 ====================
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path);
bool pointInRect(int x, int y, SDL_Rect rect);
int getPieceColor(int piece);
int getPieceType(int piece);

// 新增函数声明
bool screenToBoard(int screen_x, int screen_y, int* board_x, int* board_y);
bool movePiece(int from_x, int from_y, int to_x, int to_y);
bool isValidMove(int piece_code, int from_x, int from_y, int to_x, int to_y);
void generateNotation(ChessMove* move, int from_x, int from_y, int to_x, int to_y, int piece_code);
bool isSameColor(int piece1, int piece2);
int countPiecesInLine(int x1, int y1, int x2, int y2);
void handleBoardClick(int board_x, int board_y);
void revokeLastMove();
void drawSelectedIndicator(SDL_Renderer* renderer);
void drawCurrentPlayerIndicator(SDL_Renderer* renderer);
void drawGameInfo(SDL_Renderer* renderer);

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

// 检查点是否在矩形内
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

// ==================== 新增函数实现 ====================

// 屏幕坐标转换为棋盘坐标
bool screenToBoard(int screen_x, int screen_y, int* board_x, int* board_y) {
    // 计算相对于格点原点的坐标
    int relative_x = screen_x - GRID_ORIGIN_X;
    int relative_y = screen_y - GRID_ORIGIN_Y;
    
    // 检查是否在棋盘范围内
    if (relative_x < -PIECE_SIZE/2 || relative_x >= 9*GRID_WIDTH + PIECE_SIZE/2 ||
        relative_y < -PIECE_SIZE/2 || relative_y >= 10*GRID_HEIGHT + PIECE_SIZE/2) {
        return false;
    }
    
    // 计算最近的格点
    *board_y = (relative_x + GRID_WIDTH/2) / GRID_WIDTH;
    *board_x = (relative_y + GRID_HEIGHT/2) / GRID_HEIGHT;
    
    // 检查坐标是否在有效范围内
    if (*board_x < 0 || *board_x >= 10 || *board_y < 0 || *board_y >= 9) {
        return false;
    }
    
    return true;
}

// 检查两个棋子是否同色
bool isSameColor(int piece1, int piece2) {
    if (piece1 == NONE || piece2 == NONE) return false;
    return (getPieceColor(piece1) == getPieceColor(piece2));
}

// 统计两点之间直线上的棋子数量
int countPiecesInLine(int x1, int y1, int x2, int y2) {
    int count = 0;
    
    if (x1 == x2) {
        // 同一行
        int start_y = (y1 < y2) ? y1 : y2;
        int end_y = (y1 < y2) ? y2 : y1;
        
        for (int y = start_y + 1; y < end_y; y++) {
            if (board[x1][y] != NONE) {
                count++;
            }
        }
    } else if (y1 == y2) {
        // 同一列
        int start_x = (x1 < x2) ? x1 : x2;
        int end_x = (x1 < x2) ? x2 : x1;
        
        for (int x = start_x + 1; x < end_x; x++) {
            if (board[x][y1] != NONE) {
                count++;
            }
        }
    }
    
    return count;
}

// 生成记谱法字符串
void generateNotation(ChessMove* move, int from_x, int from_y, int to_x, int to_y, int piece_code) {
    // 简化的记谱法：只记录移动
    // 实际象棋记谱法更复杂，这里做简化
    int piece_type = getPieceType(piece_code);
    char piece_char;
    
    switch(piece_type) {
        case TYPE_JIANG: piece_char = 'K'; break;  // King
        case TYPE_SHI:   piece_char = 'A'; break;  // Advisor
        case TYPE_XIANG: piece_char = 'B'; break;  // Bishop
        case TYPE_MA:    piece_char = 'N'; break;  // kNight
        case TYPE_JU:    piece_char = 'R'; break;  // Rook
        case TYPE_PAO:   piece_char = 'C'; break;  // Cannon
        case TYPE_BING:  piece_char = 'P'; break;  // Pawn
        default:         piece_char = '?'; break;
    }
    
    // 坐标转换：象棋通常用数字1-9表示列，汉字表示行
    // 这里简化为行列数字
    snprintf(move->notation, sizeof(move->notation), "%c%d%d-%d%d", 
             piece_char, from_x, from_y, to_x, to_y);
}

// 检查移动是否合法（简化版象棋规则）
bool isValidMove(int piece_code, int from_x, int from_y, int to_x, int to_y) {
    // 不能原地不动
    if (from_x == to_x && from_y == to_y) return false;
    
    // 不能吃自己的棋子
    if (board[to_x][to_y] != NONE && isSameColor(piece_code, board[to_x][to_y])) {
        return false;
    }
    
    int color = getPieceColor(piece_code);
    int type = getPieceType(piece_code);
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    int abs_dx = abs(dx);
    int abs_dy = abs(dy);
    
    switch(type) {
        case TYPE_JIANG:  // 将/帅
            // 只能在九宫内移动
            if (color == COLOR_RED) {
                if (to_x < 7 || to_x > 9 || to_y < 3 || to_y > 5) return false;
            } else {
                if (to_x < 0 || to_x > 2 || to_y < 3 || to_y > 5) return false;
            }
            // 只能走一步
            if (abs_dx + abs_dy != 1) return false;
            return true;
            
        case TYPE_SHI:    // 士/仕
            // 只能在九宫内斜走
            if (color == COLOR_RED) {
                if (to_x < 7 || to_x > 9 || to_y < 3 || to_y > 5) return false;
            } else {
                if (to_x < 0 || to_x > 2 || to_y < 3 || to_y > 5) return false;
            }
            // 只能斜走一步
            if (abs_dx != 1 || abs_dy != 1) return false;
            return true;
            
        case TYPE_XIANG:  // 象/相
            // 不能过河
            if (color == COLOR_RED && to_x < 5) return false;
            if (color == COLOR_BLACK && to_x > 4) return false;
            // 田字走法
            if (abs_dx != 2 || abs_dy != 2) return false;
            // 检查象眼是否被塞
            int block_x = from_x + dx/2;
            int block_y = from_y + dy/2;
            if (board[block_x][block_y] != NONE) return false;
            return true;
            
        case TYPE_MA:     // 马
            // 日字走法
            if (!((abs_dx == 2 && abs_dy == 1) || (abs_dx == 1 && abs_dy == 2))) {
                return false;
            }
            // 检查蹩马腿
            if (abs_dx == 2) {
                int block_x = from_x + dx/2;
                if (board[block_x][from_y] != NONE) return false;
            } else {
                int block_y = from_y + dy/2;
                if (board[from_x][block_y] != NONE) return false;
            }
            return true;
            
        case TYPE_JU:     // 车
            // 直线移动
            if (from_x != to_x && from_y != to_y) return false;
            // 路径上不能有其他棋子
            if (countPiecesInLine(from_x, from_y, to_x, to_y) > 0) return false;
            return true;
            
        case TYPE_PAO:    // 炮
            // 直线移动
            if (from_x != to_x && from_y != to_y) return false;
            int pieces_count = countPiecesInLine(from_x, from_y, to_x, to_y);
            // 吃子时需要隔一个棋子
            if (board[to_x][to_y] != NONE) {
                return pieces_count == 1;
            } else {
                return pieces_count == 0;
            }
            
        case TYPE_BING:   // 兵/卒
            // 红方向上移动，黑方向下移动
            if (color == COLOR_RED) {
                if (dx > 0) return false;  // 红兵不能后退
                if (from_x > 4 && abs_dx > 1) return false;  // 过河前只能走一步
            } else {
                if (dx < 0) return false;  // 黑卒不能后退
                if (from_x < 5 && abs_dx > 1) return false;  // 过河前只能走一步
            }
            // 只能走一步
            if (abs_dx + abs_dy != 1) return false;
            // 不能横向移动（除非过河）
            if (abs_dy > 0) {
                if (color == COLOR_RED && from_x >= 5) return false;  // 红兵没过河
                if (color == COLOR_BLACK && from_x <= 4) return false; // 黑卒没过河
            }
            return true;
            
        default:
            return false;
    }
}

// 移动棋子
bool movePiece(int from_x, int from_y, int to_x, int to_y) {
    int piece = board[from_x][from_y];
    
    if (piece == NONE) return false;
    
    // 检查移动是否合法
    if (!isValidMove(piece, from_x, from_y, to_x, to_y)) {
        printf("非法移动！\n");
        return false;
    }
    
    // 记录吃子信息
    int captured_piece = board[to_x][to_y];
    
    // 执行移动
    board[to_x][to_y] = piece;
    board[from_x][from_y] = NONE;
    
    // 生成记谱法
    generateNotation(&current_move, from_x, from_y, to_x, to_y, piece);
    
    // 初始化棋步记录
    init_chess_move(&current_move, move_step, piece, 
                   current_move.notation, from_x, from_y, to_x, to_y);
    
    // 计算思考时间
    if (move_start_time > 0) {
        current_move.thinking_time = (int)(time(NULL) - move_start_time);
    }
    
    // 添加到棋局记录
    add_move_to_game(&current_game, &current_move);
    
    // 更新状态
    move_step++;
    is_red_turn = !is_red_turn;
    move_start_time = time(NULL);
    
    printf("移动成功！第%d步：%s %s\n", 
           current_move.step_number, current_move.piece_name, current_move.notation);
    
    // 检查是否吃子
    if (captured_piece != NONE) {
        printf("吃掉了%s！\n", get_piece_name_cn(captured_piece));
    }
    
    return true;
}

// 处理棋盘点击
void handleBoardClick(int board_x, int board_y) {
    int clicked_piece = board[board_x][board_y];
    
    if (!is_piece_selected) {
        // 第一次点击：选择棋子
        if (clicked_piece == NONE) {
            printf("点击了空位置\n");
            return;
        }
        
        // 检查是否轮到该棋子走
        int piece_color = getPieceColor(clicked_piece);
        if ((is_red_turn && piece_color != COLOR_RED) ||
            (!is_red_turn && piece_color != COLOR_BLACK)) {
            printf("现在轮到%s走棋！\n", is_red_turn ? "红方" : "黑方");
            return;
        }
        
        // 选择棋子
        is_piece_selected = true;
        selected_x = board_x;
        selected_y = board_y;
        selected_piece = clicked_piece;
        
        printf("选择了%s (%d,%d)\n", 
               get_piece_name_cn(selected_piece), selected_x, selected_y);
    } else {
        // 第二次点击：移动棋子或取消选择
        if (board_x == selected_x && board_y == selected_y) {
            // 点击同一位置：取消选择
            is_piece_selected = false;
            printf("取消选择\n");
        } else {
            // 尝试移动棋子
            if (movePiece(selected_x, selected_y, board_x, board_y)) {
                is_piece_selected = false;
            } else {
                printf("移动失败，请重新选择目标位置\n");
            }
        }
    }
}

// 悔棋功能
void revokeLastMove() {
    if (current_game.move_count == 0) {
        printf("没有棋步可以悔棋！\n");
        return;
    }
    
    ChessMove* last_move = &current_game.moves[current_game.move_count - 1];
    
    // 恢复棋盘状态
    board[last_move->from_x][last_move->from_y] = last_move->piece_code;
    board[last_move->to_x][last_move->to_y] = NONE;
    
    // 更新棋局记录
    current_game.move_count--;
    
    // 更新游戏状态
    move_step--;
    is_red_turn = !is_red_turn;
    
    printf("悔棋成功！恢复第%d步\n", last_move->step_number);
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

    // 创建窗口和渲染器
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
    // 新增保存按钮
    SDL_Texture* save_button = loadTexture(renderer, "res/images/save_button.png");
    // 新增"撤销悔棋"按钮
    SDL_Texture* redo_button = loadTexture(renderer, "res/images/redo_button.png");

    // 创建开始按钮
    SDL_Rect startButtonRect = {
        (SCREEN_WIDTH - 200) / 2,
        (SCREEN_HEIGHT - 80) / 2 + 100,
        200, 80
    };

    // 创建侧边按钮位置
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
    
    // 新增保存按钮位置
    SDL_Rect saveButtonRect = {
        30,   // 和悔棋按钮在同一列
        400,  // 在悔棋按钮下方
        100,  // 宽度
        50    // 高度
    };

    // ====== 新增："撤销悔棋"按钮位置 ======
    SDL_Rect redoButtonRect = {
        30,   // 和保存按钮在同一列
        480,  // 在保存按钮下方（400+50+30间距）
        100,  // 宽度
        50    // 高度
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
    
                // 菜单状态下点击开始按钮
                if (currentState == MENU_STATE && pointInRect(mouseX, mouseY, startButtonRect)) {
                    currentState = GAME_STATE;
                    // 初始化棋局记录
                    init_game_record(&current_game, "GAME001", "红方玩家", "黑方玩家");
                    move_start_time = time(NULL);
                    printf("新游戏开始！红方先走。\n");
                }

                // 游戏状态下点击
                if (currentState == GAME_STATE) {
                    // 检查"回到菜单"按钮
                    if (pointInRect(mouseX, mouseY, returnButtonRect)) {
                        printf("回到菜单\n");
                        currentState = MENU_STATE;
                        is_piece_selected = false;
                    }
        
                    // 检查"悔棋"按钮
                    if (pointInRect(mouseX, mouseY, revokeButtonRect)) {
                        printf("悔棋\n");
                        revokeLastMove();
                    }
                    
                    // 检查"保存棋局"按钮
                    if (pointInRect(mouseX, mouseY, saveButtonRect)) {
                        printf("保存棋局\n");
                        save_game_to_file(&current_game, "chess_game_record.txt");
                    }

                    // ====== 新增：检查"撤销悔棋"按钮 ======
                    if (pointInRect(mouseX, mouseY, redoButtonRect)) {
                        printf("撤销悔棋\n");
                        // TODO: 这里将来实现撤销悔棋功能
                        // redoLastMove();
                    }
                    
                    // 检查是否点击了棋盘
                    int board_x, board_y;
                    if (screenToBoard(mouseX, mouseY, &board_x, &board_y)) {
                        handleBoardClick(board_x, board_y);
                    }
                }
            }
            
            // 按ESC返回菜单
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                if (currentState == GAME_STATE) {
                    currentState = MENU_STATE;
                    is_piece_selected = false;
                }
            }
            
            // 按S键保存棋局
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s) {
                if (currentState == GAME_STATE) {
                    save_game_to_file(&current_game, "chess_game_record.txt");
                }
            }
            
            // 按R键悔棋
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                if (currentState == GAME_STATE) {
                    revokeLastMove();
                }
            }

            // ====== 新增：按D键撤销悔棋 ======
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_d) {
                if (currentState == GAME_STATE) {
                    printf("撤销悔棋 (快捷键D)\n");
                    // TODO: 这里将来实现撤销悔棋功能
                    // redoLastMove();
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
                // 如果没有棋盘图片，绘制简单棋盘
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
            
            // 绘制选中指示器
            drawSelectedIndicator(renderer);
            
            // 绘制当前玩家指示器
            drawCurrentPlayerIndicator(renderer);
            
            // 绘制游戏信息
            drawGameInfo(renderer);

            // 侧边按钮渲染代码
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
            
            // 渲染"保存棋局"按钮
            if (save_button) {
                SDL_RenderCopy(renderer, save_button, NULL, &saveButtonRect);
            } else {
                // 如果图标加载失败，绘制默认按钮
                SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
                SDL_RenderFillRect(renderer, &saveButtonRect);
                // 绘制边框
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &saveButtonRect);
            }

            // ====== 新增：渲染"撤销悔棋"按钮 ======
            if (redo_button) {
                SDL_RenderCopy(renderer, redo_button, NULL, &redoButtonRect);
            } else {
                // 如果图标加载失败，绘制默认按钮
                SDL_SetRenderDrawColor(renderer, 200, 150, 50, 255);  // 橙色
                SDL_RenderFillRect(renderer, &redoButtonRect);
                // 绘制边框
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &redoButtonRect);
            }
        }

        // 呈现画面
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // 清理资源
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

    // 清理侧边按钮纹理
    if (return_button) SDL_DestroyTexture(return_button);
    if (revoke_button) SDL_DestroyTexture(revoke_button);
    if (save_button) SDL_DestroyTexture(save_button);
    if (redo_button) SDL_DestroyTexture(redo_button);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    printf("游戏退出\n");
    return 0;
}