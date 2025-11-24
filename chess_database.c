#include "chess_database.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// ==================== 棋子编码工具函数 ====================

// 编码：颜色 + 类型 → 数字
int encode_piece(int color, int piece_type) {
    return color * 10 + piece_type;
}

// 解码：获取棋子颜色
int get_piece_color(int piece_code) {
    return piece_code / 10;
}

// 解码：获取棋子类型
int get_piece_type(int piece_code) {
    return piece_code % 10;
}

// 根据编码获取中文名
const char* get_piece_name_cn(int piece_code) {
    int color = get_piece_color(piece_code);
    int type = get_piece_type(piece_code);
    
    // 红方棋子名
    if (color == COLOR_RED) {
        switch(type) {
            case TYPE_JIANG: return "帅";
            case TYPE_SHI:   return "士";
            case TYPE_XIANG: return "相";
            case TYPE_MA:    return "马";
            case TYPE_JU:    return "车";
            case TYPE_PAO:   return "炮";
            case TYPE_BING:  return "兵";
            default:         return "未知";
        }
        // 黑方棋子名
    } else {
        switch(type) {
            case TYPE_JIANG: return "将";
            case TYPE_SHI:   return "仕";
            case TYPE_XIANG: return "象";
            case TYPE_MA:    return "马";
            case TYPE_JU:    return "车";
            case TYPE_PAO:   return "炮";
            case TYPE_BING:  return "卒";
            default:         return "未知";
        }
    }
}

// ==================== 棋步管理函数 ====================

void init_chess_move(ChessMove* move, int step, int piece_code, 
                     const char* notation, int from_x, int from_y, 
                     int to_x, int to_y) {
    move->step_number = step;
    move->piece_code = piece_code;
    
    // 安全复制字符串
    strncpy(move->notation, notation, sizeof(move->notation) - 1);
    move->notation[sizeof(move->notation) - 1] = '\0';
    
    const char* name = get_piece_name_cn(piece_code);
    strncpy(move->piece_name, name, sizeof(move->piece_name) - 1);
    move->piece_name[sizeof(move->piece_name) - 1] = '\0';
    
    // 设置坐标（新坐标系：x:0-9行, y:0-8列）
    move->from_x = from_x;
    move->from_y = from_y;
    move->to_x = to_x;
    move->to_y = to_y;
    
    get_current_timestamp(move->timestamp, sizeof(move->timestamp));
    move->thinking_time = 0;
}

// ==================== 棋局管理函数 ====================

void init_game_record(GameRecord* game, const char* id, 
                      const char* red_player, const char* black_player) {
     // 安全复制字符串
    strncpy(game->game_id, id, sizeof(game->game_id) - 1);
    game->game_id[sizeof(game->game_id) - 1] = '\0';
    
    strncpy(game->player_red, red_player, sizeof(game->player_red) - 1);
    game->player_red[sizeof(game->player_red) - 1] = '\0';
    
    strncpy(game->player_black, black_player, sizeof(game->player_black) - 1);
    game->player_black[sizeof(game->player_black) - 1] = '\0';
    
    // 生成棋局标题
    snprintf(game->game_title, sizeof(game->game_title), 
             "%s vs %s", red_player, black_player);
    
     // 初始化其他字段
    game->move_count = 0;
    game->result = 0;
    strcpy(game->result_str, "未结束");
    
    game->start_time = time(NULL);
    game->end_time = 0;
    
    game->total_red_thinking = 0;
    game->total_black_thinking = 0;
}

void add_move_to_game(GameRecord* game, const ChessMove* move) {
    if (game->move_count >= 500) {
        printf("警告：棋局步数已达上限！\n");
        return;
    }
    
    game->moves[game->move_count] = *move;
    game->move_count++;
    
    // 更新思考时间统计
    int color = get_piece_color(move->piece_code);
    if (color == COLOR_RED) {
        game->total_red_thinking += move->thinking_time;
    } else {
        game->total_black_thinking += move->thinking_time;
    }
    
    const char* player_name = (color == COLOR_RED) ? "红方" : "黑方";
    printf("已记录第%d步: %s %s %s ", 
           move->step_number, player_name, move->piece_name, move->notation);
    printf("(%d,%d)→(%d,%d)\n", move->from_x, move->from_y, move->to_x, move->to_y);
}

void set_game_result(GameRecord* game, int result) {
    game->result = result;
    game->end_time = time(NULL);
    
    switch(result) {
        case 1: strcpy(game->result_str, "红胜"); break;
        case 2: strcpy(game->result_str, "黑胜"); break;
        case 3: strcpy(game->result_str, "平局"); break;
        default: strcpy(game->result_str, "未结束");
    }
}

// ==================== 工具函数 ====================

void print_game_summary(const GameRecord* game) {
    printf("\n=== 棋局摘要 ===\n");
    printf("棋局ID: %s\n", game->game_id);
    printf("对局: %s\n", game->game_title);
    printf("总步数: %d\n", game->move_count);
    printf("结果: %s\n", game->result_str);
    
    if (game->move_count > 0) {
        printf("红方总思考: %d秒\n", game->total_red_thinking);
        printf("黑方总思考: %d秒\n", game->total_black_thinking);
    }
    printf("================\n\n");
}

void save_game_to_file(const GameRecord* game, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("无法创建文件: %s\n", filename);
        return;
    }
    
    fprintf(file, "棋局ID: %s\n", game->game_id);
    fprintf(file, "红方: %s\n", game->player_red);
    fprintf(file, "黑方: %s\n", game->player_black);
    fprintf(file, "结果: %s\n", game->result_str);
    fprintf(file, "总步数: %d\n\n", game->move_count);
    
    fprintf(file, "步数,玩家,棋子,走法,起始坐标,目标坐标,时间\n");
    
    for (int i = 0; i < game->move_count; i++) {
        const ChessMove* move = &game->moves[i];
        const char* player = (get_piece_color(move->piece_code) == COLOR_RED) ? "红方" : "黑方";
        
        fprintf(file, "%d,%s,%s,%s,(%d,%d),(%d,%d),%s\n",
                move->step_number, player, move->piece_name, move->notation,
                move->from_x, move->from_y, move->to_x, move->to_y,
                move->timestamp);
    }
    
    fclose(file);
    printf("棋局已保存到: %s\n", filename);
}

void get_current_timestamp(char* buffer, int size) {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

// 坐标调试函数
void print_coordinates(int x, int y) {
    printf("坐标: 第%d行, 第%d列\n", x, y);
}
