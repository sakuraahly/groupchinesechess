#ifndef CHESS_DATABASE_H
#define CHESS_DATABASE_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#pragma once

// ==================== 棋子编码系统 ====================
#define COLOR_RED    1
#define COLOR_BLACK  2

#define TYPE_JIANG   1
#define TYPE_SHI     2
#define TYPE_XIANG   3
#define TYPE_MA      4
#define TYPE_JU      5
#define TYPE_PAO     6
#define TYPE_BING    7

// 红方棋子
#define RED_JIANG    11
#define RED_SHI      12
#define RED_XIANG    13
#define RED_MA       14
#define RED_JU       15
#define RED_PAO      16
#define RED_BING     17

// 黑方棋子
#define BLACK_JIANG  21
#define BLACK_SHI    22
#define BLACK_XIANG  23
#define BLACK_MA     24
#define BLACK_JU     25
#define BLACK_PAO    26
#define BLACK_BING   27

// ==================== 棋盘常量 ====================
#define BOARD_ROWS    10
#define BOARD_COLS    9

// ==================== 数据结构 ====================
typedef struct {
    int step_number;
    int piece_code;
    char piece_name[8];
    char notation[16];
    int from_x, from_y;
    int to_x, to_y;
    char timestamp[32];
    int thinking_time;
} ChessMove;

typedef struct {
    char game_id[24];
    char player_red[24];
    char player_black[24];
    char game_title[64];
    ChessMove moves[500];
    int result;
    int move_count;
    char result_str[16];
    time_t start_time;
    time_t end_time;
    int total_red_thinking;
    int total_black_thinking;
} GameRecord;

// ==================== 函数声明 ====================
int encode_piece(int color, int piece_type);
int get_piece_color(int piece_code);
int get_piece_type(int piece_code);
const char* get_piece_name_cn(int piece_code);

void init_chess_move(ChessMove* move, int step, int piece_code, 
                     const char* notation, int from_x, int from_y, 
                     int to_x, int to_y);

void init_game_record(GameRecord* game, const char* id, 
                      const char* red_player, const char* black_player);
void add_move_to_game(GameRecord* game, const ChessMove* move);
void set_game_result(GameRecord* game, int result);

void print_game_summary(const GameRecord* game);
void save_game_to_file(const GameRecord* game, const char* filename);
void get_current_timestamp(char* buffer, int size);

#endif