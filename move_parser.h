#ifndef MOVE_PARSER_H
#define MOVE_PARSER_H

#include "chess_database.h"

#pragma once

typedef struct {
    int valid;
    int piece_code;
    int from_x, from_y;
    int to_x, to_y;
    char error_msg[64];
} ParseResult;

ParseResult parse_notation(const char* notation, int player_color, const GameRecord* game);
void print_parse_result(const ParseResult* result);

#endif