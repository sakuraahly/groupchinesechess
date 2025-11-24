#ifndef CHESS_DATABASE_H
#define CHESS_DATABASE_H

#include <stdio.h>
#include <string.h>
#include <time.h>

// 使用编译指令加强保护
#pragma once

// ==================== 棋子编码系统 ====================
// 两位数编码：十位表示颜色，个位表示棋子种类

// 颜色编码（十位）
#define COLOR_RED    1  // 红方
#define COLOR_BLACK  2  // 黑方

// 棋子种类编码（个位）
#define TYPE_JIANG   1  // 将/帅
#define TYPE_SHI     2  // 士/仕
#define TYPE_XIANG   3  // 相/象
#define TYPE_MA      4  // 马
#define TYPE_JU      5  // 车
#define TYPE_PAO     6  // 炮
#define TYPE_BING    7  // 兵/卒

// 具体棋子编码
// 红方棋子
#define RED_JIANG    11  // 红帅
#define RED_SHI      12  // 红士
#define RED_XIANG    13  // 红相
#define RED_MA       14  // 红马
#define RED_JU       15  // 红车
#define RED_PAO      16  // 红炮
#define RED_BING     17  // 红兵

// 黑方棋子
#define BLACK_JIANG  21  // 黑将
#define BLACK_SHI    22  // 黑仕
#define BLACK_XIANG  23  // 黑象
#define BLACK_MA     24  // 黑马
#define BLACK_JU     25  // 黑车
#define BLACK_PAO    26  // 黑炮
#define BLACK_BING   27  // 黑卒

// ==================== 棋盘常量 ====================
#define BOARD_ROWS    10  // x坐标：0-9行 (从上到下)
#define BOARD_COLS    9   // y坐标：0-8列 (从左到右)

// ==================== 数据结构 ====================

// 单步棋记录结构体
typedef struct {
    int step_number;            // 步数编号
    int piece_code;             // 棋子编码（两位数）
    char piece_name[8];         // 棋子中文名
    char notation[16];          // 记谱法
    
    // 坐标信息 (x:0-9行, y:0-8列) - 左上角(0,0)为黑车
    int from_x, from_y;         // 起始位置
    int to_x, to_y;             // 目标位置
    
    // 时间信息
    char timestamp[32];         // 格式："2023-11-24 14:30:25"
    
    int thinking_time;          // 思考时间(秒)
} ChessMove;

// 棋局记录结构体
typedef struct {
    char game_id[24];           // 棋局ID
    char player_red[24];        // 红方玩家名
    char player_black[24];      // 黑方玩家名
    char game_title[64];        // 棋局标题
    
    ChessMove moves[500];       // 棋步记录数组
    int move_count;             // 总步数
    
    // 棋局结果
    int result;                 // 0=未结束, 1=红胜, 2=黑胜, 3=平局
    char result_str[16];        // 结果字符串
    
    time_t start_time;          // 开始时间
    time_t end_time;            // 结束时间
    
    // 统计信息
    int total_red_thinking;     // 红方总思考时间
    int total_black_thinking;   // 黑方总思考时间
} GameRecord;

// ==================== 函数声明 ====================

// 棋子编码工具函数
int encode_piece(int color, int piece_type);
int get_piece_color(int piece_code);
int get_piece_type(int piece_code);
const char* get_piece_name_cn(int piece_code);

// 棋步管理函数
void init_chess_move(ChessMove* move, int step, int piece_code, 
                     const char* notation, int from_x, int from_y, 
                     int to_x, int to_y);

// 棋局管理函数
void init_game_record(GameRecord* game, const char* id, 
                      const char* red_player, const char* black_player);
void add_move_to_game(GameRecord* game, const ChessMove* move);
void set_game_result(GameRecord* game, int result);

// 工具函数
void print_game_summary(const GameRecord* game);
void save_game_to_file(const GameRecord* game, const char* filename);
void get_current_timestamp(char* buffer, int size);

// 坐标工具函数
void print_coordinates(int x, int y);  // 调试用

#endif // CHESS_DATABASE_H
