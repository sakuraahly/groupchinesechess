#ifndef MOVE_PARSER_H
#define MOVE_PARSER_H

#include "chess_database.h"

#pragma once

// 解析结果结构体：存储记谱法解析的结果
typedef struct {
    int valid;              // 解析是否成功（1:成功, 0:失败）
    int piece_code;         // 棋子编码
    int from_x, from_y;     // 起始坐标
    int to_x, to_y;         // 目标坐标
    char error_msg[64];     // 错误信息（解析失败时使用）
} ParseResult;

// 函数声明
ParseResult parse_notation(const char* notation, int player_color, const GameRecord* game);
void print_parse_result(const ParseResult* result);

#endif