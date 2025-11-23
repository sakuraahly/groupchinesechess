#include "chess_database.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// 初始化单步棋记录
void init_chess_move(ChessMove* move, int step, PlayerColor player, 
                     PieceType type, const char* notation,
                     int from_x, int from_y, int to_x, int to_y) {
    move->step_number = step;
    move->player = player;
    move->piece_type = type;
    
    // 复制记谱字符串
    strncpy(move->notation, notation, sizeof(move->notation) - 1);
    move->notation[sizeof(move->notation) - 1] = '\0';
    
    // 设置棋子中文名
    const char* name = get_piece_name_cn(type);
    strncpy(move->piece_name, name, sizeof(move->piece_name) - 1);
    move->piece_name[sizeof(move->piece_name) - 1] = '\0';
    
    // 设置坐标
    move->from_x = from_x;
    move->from_y = from_y;
    move->to_x = to_x;
    move->to_y = to_y;
    
    // 设置时间戳
    move->timestamp = time(NULL);
    get_current_timestamp(move->time_str, sizeof(move->time_str));
    
    move->thinking_time = 0;  // 初始思考时间为0
}

// 初始化棋局记录
void init_game_record(GameRecord* game, const char* id, 
                      const char* red_player, const char* black_player) {
    // 设置棋局ID
    strncpy(game->game_id, id, sizeof(game->game_id) - 1);
    game->game_id[sizeof(game->game_id) - 1] = '\0';
    
    // 设置玩家名
    strncpy(game->player_red, red_player, sizeof(game->player_red) - 1);
    game->player_red[sizeof(game->player_red) - 1] = '\0';
    
    strncpy(game->player_black, black_player, sizeof(game->player_black) - 1);
    game->player_black[sizeof(game->player_black) - 1] = '\0';
    
    // 生成棋局标题
    snprintf(game->game_title, sizeof(game->game_title), 
             "%s vs %s", red_player, black_player);
    
    // 初始化其他字段
    game->move_count = 0;
    game->result = 0;  // 未结束
    strcpy(game->result_str, "未结束");
    
    game->start_time = time(NULL);
    game->end_time = 0;
    
    game->total_red_thinking = 0;
    game->total_black_thinking = 0;
}

// 添加棋步到棋局
void add_move_to_game(GameRecord* game, const ChessMove* move) {
    if (game->move_count >= 500) {
        printf("警告：棋局步数已达上限500步！\n");
        return;
    }
    
    // 复制棋步信息
    game->moves[game->move_count] = *move;
    game->move_count++;
    
    // 更新思考时间统计
    if (move->player == PLAYER_RED) {
        game->total_red_thinking += move->thinking_time;
    } else {
        game->total_black_thinking += move->thinking_time;
    }
    
    printf("已记录第%d步: %s %s\n", 
           move->step_number,
           get_player_name_cn(move->player),
           move->notation);
}

// 设置棋局结果
void set_game_result(GameRecord* game, int result) {
    game->result = result;
    game->end_time = time(NULL);
    
    switch(result) {
        case 1:
            strcpy(game->result_str, "红胜");
            break;
        case 2:
            strcpy(game->result_str, "黑胜");
            break;
        case 3:
            strcpy(game->result_str, "平局");
            break;
        default:
            strcpy(game->result_str, "未结束");
    }
    
    printf("棋局结果已设置: %s\n", game->result_str);
}

// 根据棋子类型返回中文名
const char* get_piece_name_cn(PieceType type) {
    static const char* names[] = {
        "帅", "士", "相", "马", "车", "炮", "兵"
    };
    
    if (type >= PIECE_KING && type <= PIECE_PAWN) {
        return names[type];
    }
    return "未知";
}

// 根据玩家阵营返回中文名
const char* get_player_name_cn(PlayerColor player) {
    return (player == PLAYER_RED) ? "红方" : "黑方";
}

// 获取当前时间戳字符串
void get_current_timestamp(char* buffer, int size) {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

// 打印单步棋信息（调试用）
void print_chess_move(const ChessMove* move) {
    printf("第%d步 %s %s %s ", 
           move->step_number,
           get_player_name_cn(move->player),
           move->piece_name,
           move->notation);
    
    printf("(%d,%d)→(%d,%d) ", 
           move->from_x, move->from_y, 
           move->to_x, move->to_y);
    
    printf("时间: %s\n", move->time_str);
}

// 打印完整棋局信息
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

// 保存棋局到文件（基础版本）
int save_game_to_file(const GameRecord* game, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("无法创建文件: %s\n", filename);
        return 0;
    }
    
    // 写入棋局头信息
    fprintf(file, "棋局ID: %s\n", game->game_id);
    fprintf(file, "红方: %s\n", game->player_red);
    fprintf(file, "黑方: %s\n", game->player_black);
    fprintf(file, "结果: %s\n", game->result_str);
    fprintf(file, "总步数: %d\n", game->move_count);
    fprintf(file, "\n");
    
    // 写入列标题
    fprintf(file, "步数,玩家,棋子,走法,起始,目标,时间\n");
    
    // 写入每一步棋
    for (int i = 0; i < game->move_count; i++) {
        const ChessMove* move = &game->moves[i];
        fprintf(file, "%d,%s,%s,%s,(%d,%d),(%d,%d),%s\n",
                move->step_number,
                get_player_name_cn(move->player),
                move->piece_name,
                move->notation,
                move->from_x, move->from_y,
                move->to_x, move->to_y,
                move->time_str);
    }
    
    fclose(file);
    printf("棋局已保存到: %s\n", filename);
    return 1;
}