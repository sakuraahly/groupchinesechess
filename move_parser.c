#include "move_parser.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// 中文数字转阿拉伯数字
int chinese_number_to_int(const char* chinese_num) {
    const char* numbers[] = {"一", "二", "三", "四", "五", "六", "七", "八", "九"};
    for (int i = 0; i < 9; i++) {
        if (strcmp(chinese_num, numbers[i]) == 0) {
            return i + 1;  // 一→1, 二→2, ..., 九→9
        }
    }
    return -1;  // 无效数字
}

// 根据棋子名称返回棋子类型
const char* get_piece_type_from_name(const char* piece_name) {
    if (strcmp(piece_name, "车") == 0) return "Chariot";
    if (strcmp(piece_name, "马") == 0) return "Horse";
    if (strcmp(piece_name, "炮") == 0) return "Cannon";
    if (strcmp(piece_name, "相") == 0 || strcmp(piece_name, "象") == 0) return "Elephant";
    if (strcmp(piece_name, "士") == 0 || strcmp(piece_name, "仕") == 0) return "Advisor";
    if (strcmp(piece_name, "帅") == 0 || strcmp(piece_name, "将") == 0) return "King";
    if (strcmp(piece_name, "兵") == 0 || strcmp(piece_name, "卒") == 0) return "Pawn";
    return "Unknown";
}

// 核心解析函数
ParseResult parse_notation(const char* notation, PlayerColor player, const GameRecord* game) {
    ParseResult result = {0};
    result.valid = 0;  // 默认解析失败
    
    // 基本长度检查
    if (strlen(notation) < 3) {
        strcpy(result.error_msg, "记谱太短");
        return result;
    }
    
    // 提取棋子名称（第一个字符）
    char piece_name[4] = {0};
    strncpy(piece_name, notation, 3);  // 中文字符可能占3字节
    
    // 简化版解析：假设格式为 "兵五进一"
    // 实际实现需要更复杂的逻辑...
    
    // 这里先返回一个示例结果
    result.valid = 1;
    result.piece_type = PIECE_PAWN;  // 假设是兵
    
    // 红黑方坐标映射（简化版）
    if (player == PLAYER_RED) {
        // 红方：从底部向上，右边是一
        result.from_x = 4;  // 假设第五列
        result.from_y = 3;  // 红方兵初始位置
        result.to_x = 4;
        result.to_y = 4;    // 前进一格
    } else {
        // 黑方：从顶部向下，左边是一  
        result.from_x = 4;  // 假设第五列
        result.from_y = 6;  // 黑方卒初始位置
        result.to_x = 4;
        result.to_y = 5;    // 前进一格（向下）
    }
    
    strcpy(result.error_msg, "解析成功");
    return result;
}

// 打印解析结果
void print_parse_result(const ParseResult* result) {
    if (result->valid) {
        printf("解析成功: (%d,%d) -> (%d,%d)\n", 
               result->from_x, result->from_y,
               result->to_x, result->to_y);
    } else {
        printf("解析失败: %s\n", result->error_msg);
    }
}