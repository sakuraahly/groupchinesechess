#ifndef MOVE_PARSER_H
#define MOVE_PARSER_H

#include "chess_database.h"

// 解析结果结构体
typedef struct {
    int valid;              // 是否解析成功
    PieceType piece_type;   // 棋子类型
    int from_x, from_y;     // 起始位置
    int to_x, to_y;         // 目标位置
    char error_msg[50];     // 错误信息
} ParseResult;

// 函数声明
ParseResult parse_notation(const char* notation, PlayerColor player, const GameRecord* game);
int chinese_number_to_int(const char* chinese_num);
const char* get_piece_type_from_name(const char* piece_name);
void print_parse_result(const ParseResult* result);

#endif // MOVE_PARSER_H