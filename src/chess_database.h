#ifndef CHESS_DATABASE_H
#define CHESS_DATABASE_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#pragma once

// ==================== 棋子编码系统 ====================
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
#define BLACK_ZU   27  // 黑卒

// ==================== 棋盘常量 ====================
#define BOARD_ROWS    10  // 棋盘行数
#define BOARD_COLS    9   // 棋盘列数

// ==================== 数据结构 ====================
// 棋步结构体：记录每一步棋的详细信息
typedef struct {
    int step_number;        // 步数编号
    int piece_code;         // 棋子编码
    char piece_name[8];     // 棋子中文名称
    char notation[16];      // 记谱法表示
    int from_x, from_y;     // 起始坐标
    int to_x, to_y;         // 目标坐标
    char timestamp[32];     // 时间戳
    int thinking_time;      // 思考时间（秒）
} ChessMove;

// 棋局记录结构体：记录整盘棋的完整信息
typedef struct {
    char game_id[24];           // 棋局唯一标识
    char player_red[24];        // 红方玩家名称
    char player_black[24];      // 黑方玩家名称
    char game_title[64];        // 棋局标题
    ChessMove moves[500];       // 棋步记录数组（最大500步）
    int result;                 // 棋局结果（1:红胜, 2:黑胜, 3:平局）
    int move_count;             // 实际步数
    char result_str[16];        // 结果字符串
    time_t start_time;          // 开始时间
    time_t end_time;            // 结束时间
    int total_red_thinking;     // 红方总思考时间
    int total_black_thinking;   // 黑方总思考时间
} GameRecord;

// ==================== 函数声明 ====================
// 棋子编码相关函数
int encode_piece(int color, int piece_type);
int get_piece_color(int piece_code);
int get_piece_type(int piece_code);
const char* get_piece_name_cn(int piece_code);

// 棋步操作函数
void init_chess_move(ChessMove* move, int step, int piece_code, 
                     const char* notation, int from_x, int from_y, 
                     int to_x, int to_y);

// 棋局操作函数
void init_game_record(GameRecord* game, const char* id, 
                      const char* red_player, const char* black_player);
void add_move_to_game(GameRecord* game, const ChessMove* move);
void set_game_result(GameRecord* game, int result);

// 输出和保存函数
void print_game_summary(const GameRecord* game);
void save_game_to_file(const GameRecord* game, const char* filename);
void get_current_timestamp(char* buffer, int size);

#endif