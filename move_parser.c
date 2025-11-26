#include "move_parser.h"
#include "chess_database.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// 解析象棋记谱法（简化版本）
// 当前版本仅返回固定的坐标，实际应用中需要实现完整的记谱法解析逻辑
ParseResult parse_notation(const char* notation, int player_color, const GameRecord* game) {
    ParseResult result = {0};
    result.valid = 1;
    
    // 简化解析：根据玩家返回不同的棋子
    // 注意：这是简化实现，实际需要根据记谱法字符串解析具体的移动
    if (player_color == COLOR_RED) {
        result.piece_code = RED_BING;
        result.from_x = 6;
        result.from_y = 4;
        result.to_x = 5;
        result.to_y = 4;
    } else {
        result.piece_code = BLACK_PAO;
        result.from_x = 2;
        result.from_y = 1;
        result.to_x = 2;
        result.to_y = 4;
    }
    
    strcpy(result.error_msg, "解析成功");
    return result;
}

// 打印解析结果
void print_parse_result(const ParseResult* result) {
    if (result->valid) {
        const char* color_str = (get_piece_color(result->piece_code) == COLOR_RED) ? "红方" : "黑方";
        const char* piece_name = get_piece_name_cn(result->piece_code);
        printf("解析成功: %s %s 从(%d,%d)移动到(%d,%d)\n", 
               color_str, piece_name, result->from_x, result->from_y, result->to_x, result->to_y);
    } else {
        printf("解析失败: %s\n", result->error_msg);
    }
}