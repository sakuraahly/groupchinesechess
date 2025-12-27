#include "chess_database.h"
#include "chess_move.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define NONE 0

 GameRecord current_game;        // 当前棋局记录
 ChessMove current_move;         // 当前棋步记录
 bool is_red_turn = true;        // 当前轮到红方走棋
 int move_step = 1;              // 当前步数计数器
 time_t move_start_time = 0;     // 当前步开始时间

// 棋子选择状态
 bool is_piece_selected = false;
 int selected_x = -1;
 int selected_y = -1;
 int selected_piece = NONE;

// 将颜色和棋子类型编码为一个整数
// 编码规则：颜色 * 10 + 棋子类型
int encode_piece(int color, int piece_type) {
    return color * 10 + piece_type;
}

// 从棋子编码中提取颜色信息
int get_piece_color(int piece_code) {
    return piece_code / 10;
}

// 从棋子编码中提取棋子类型信息
int get_piece_type(int piece_code) {
    return piece_code % 10;
}

// 根据棋子编码获取中文棋子名称
const char* get_piece_name_cn(int piece_code) {
    int color = get_piece_color(piece_code);
    int type = get_piece_type(piece_code);
    
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

// 初始化棋步结构体
void init_chess_move(ChessMove* move, int step, int piece_code, 
                     const char* notation, int from_x, int from_y, 
                     int to_x, int to_y, int captured_piece) { // 新增参数 captured_piece
    move->step_number = step;
    move->piece_code = piece_code;
    
    // 安全地复制记谱字符串
    strncpy(move->notation, notation, sizeof(move->notation) - 1);
    move->notation[sizeof(move->notation) - 1] = '\0';
    
    // 获取并存储棋子中文名称
    const char* name = get_piece_name_cn(piece_code);
    strncpy(move->piece_name, name, sizeof(move->piece_name) - 1);
    move->piece_name[sizeof(move->piece_name) - 1] = '\0';
    
    // 设置坐标信息
    move->from_x = from_x;
    move->from_y = from_y;
    move->to_x = to_x;
    move->to_y = to_y;
    
    move->captured_piece = captured_piece;  // 新增：保存被吃掉的棋子

    // 生成时间戳并初始化思考时间
    get_current_timestamp(move->timestamp, sizeof(move->timestamp));
    move->thinking_time = 0;
}

// 初始化棋局记录
void init_game_record(GameRecord* game, const char* id, 
                      const char* red_player, const char* black_player) {
    // 复制棋局ID
    strncpy(game->game_id, id, sizeof(game->game_id) - 1);
    game->game_id[sizeof(game->game_id) - 1] = '\0';
    
    // 复制红方玩家名称
    strncpy(game->player_red, red_player, sizeof(game->player_red) - 1);
    game->player_red[sizeof(game->player_red) - 1] = '\0';
    
    // 复制黑方玩家名称
    strncpy(game->player_black, black_player, sizeof(game->player_black) - 1);
    game->player_black[sizeof(game->player_black) - 1] = '\0';
    
    // 生成棋局标题
    snprintf(game->game_title, sizeof(game->game_title), 
             "%s vs %s", red_player, black_player);
    
    // 初始化棋局状态
    game->move_count = 0;
    game->result = 0;
    strcpy(game->result_str, "未结束");
    
    // 设置开始时间
    game->start_time = time(NULL);
    game->end_time = 0;
    
    // 初始化思考时间统计
    game->total_red_thinking = 0;
    game->total_black_thinking = 0;
}

// 添加棋步到棋局记录中
void add_move_to_game(GameRecord* game, const ChessMove* move) {
    // 检查步数是否超过上限
    if (game->move_count >= 500) {
        printf("警告：棋局步数已达上限！\n");
        return;
    }
    
    // 添加棋步到数组中
    game->moves[game->move_count] = *move;
    game->move_count++;
    
    // 根据棋子颜色累加思考时间
    int color = get_piece_color(move->piece_code);
    if (color == COLOR_RED) {
        game->total_red_thinking += move->thinking_time;
    } else {
        game->total_black_thinking += move->thinking_time;
    }
    
    // 输出添加的棋步信息
    const char* player_name = (color == COLOR_RED) ? "红方" : "黑方";
    printf("已记录第%d步: %s %s %s ", 
           move->step_number, player_name, move->piece_name, move->notation);
    printf("(%d,%d)→(%d,%d)\n", move->from_x, move->from_y, move->to_x, move->to_y);
}

// 设置棋局结果
void set_game_result(GameRecord* game, int result) {
    game->result = result;
    game->end_time = time(NULL);
    
    // 根据结果代码设置结果字符串
    switch(result) {
        case 1: strcpy(game->result_str, "红胜"); break;
        case 2: strcpy(game->result_str, "黑胜"); break;
        case 3: strcpy(game->result_str, "平局"); break;
        default: strcpy(game->result_str, "未结束");
    }
}

// 打印棋局摘要信息
void print_game_summary(const GameRecord* game) {
    printf("\n=== 棋局摘要 ===\n");
    printf("棋局ID: %s\n", game->game_id);
    printf("对局: %s\n", game->game_title);
    printf("总步数: %d\n", game->move_count);
    printf("结果: %s\n", game->result_str);
    
    // 如果有棋步记录，显示思考时间统计
    if (game->move_count > 0) {
        printf("红方总思考: %d秒\n", game->total_red_thinking);
        printf("黑方总思考: %d秒\n", game->total_black_thinking);
    }
    printf("================\n\n");
}

// 将棋局保存到文件
void save_game_to_file(const GameRecord* game, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("无法创建文件: %s\n", filename);
        return;
    }
    
    // 写入棋局基本信息
    fprintf(file, "棋局ID: %s\n", game->game_id);
    fprintf(file, "红方: %s\n", game->player_red);
    fprintf(file, "黑方: %s\n", game->player_black);
    fprintf(file, "结果: %s\n", game->result_str);
    fprintf(file, "总步数: %d\n\n", game->move_count);
    
    // 写入表头
    fprintf(file, "步数,玩家,棋子,走法,起始坐标,目标坐标,时间\n");
    
    // 遍历所有棋步并写入文件
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

// 获取当前时间戳字符串
void get_current_timestamp(char* buffer, int size) {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

// 生成记谱法字符串
void generateNotation(ChessMove* move, int from_x, int from_y, int to_x, int to_y, int piece_code) {
    // 简化的记谱法：只记录移动
    // 实际象棋记谱法更复杂，这里做简化
    int piece_type = getPieceType(piece_code);
    char piece_char;
    
    switch(piece_type) {
        case TYPE_JIANG: piece_char = 'K'; break;  // King
        case TYPE_SHI:   piece_char = 'A'; break;  // Advisor
        case TYPE_XIANG: piece_char = 'B'; break;  // Bishop
        case TYPE_MA:    piece_char = 'N'; break;  // kNight
        case TYPE_JU:    piece_char = 'R'; break;  // Rook
        case TYPE_PAO:   piece_char = 'C'; break;  // Cannon
        case TYPE_BING:  piece_char = 'P'; break;  // Pawn
        default:         piece_char = '?'; break;
    }
    
    // 坐标转换：象棋通常用数字1-9表示列，汉字表示行
    // 这里简化为行列数字
    snprintf(move->notation, sizeof(move->notation), "%c%d%d-%d%d", 
             piece_char, from_x, from_y, to_x, to_y);
}

// ==================== 存档系统 ====================

// 检查存档是否存在
bool has_save() {
    FILE* file = fopen(SAVE_FILE, "rb");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// 保存当前游戏（覆盖保存）
void save_game() {
    FILE* file = fopen(SAVE_FILE, "wb");
    if (!file) {
        printf("保存失败：无法创建存档文件\n");
        return;
    }
    
    // 保存最重要的6部分数据：
    
    // 1. 保存棋盘状态（10行×9列）
    fwrite(board, sizeof(int), 10 * 9, file);
    
    // 2. 保存游戏记录
    fwrite(&current_game, sizeof(GameRecord), 1, file);
    
    // 3. 保存游戏状态
    fwrite(&is_red_turn, sizeof(bool), 1, file);
    fwrite(&move_step, sizeof(int), 1, file);
    
    // 4. 保存棋子选择状态（用于恢复选中状态）
    fwrite(&is_piece_selected, sizeof(bool), 1, file);
    fwrite(&selected_x, sizeof(int), 1, file);
    fwrite(&selected_y, sizeof(int), 1, file);
    fwrite(&selected_piece, sizeof(int), 1, file);
    
    // 5. 新增：保存游戏胜负状态
    fwrite(&is_shuai_live, sizeof(bool), 1, file);
    fwrite(&is_jiang_live, sizeof(bool), 1, file);
    fwrite(&redFlyToWin, sizeof(bool), 1, file);
    fwrite(&blackFlyToWin, sizeof(bool), 1, file);
    
    fclose(file);
    
    printf("游戏已保存\n");
    printf("步数：%d，轮到：%s\n", 
           current_game.move_count, 
           is_red_turn ? "红方" : "黑方");
}

// 加载存档
bool load_game() {
    FILE* file = fopen(SAVE_FILE, "rb");
    if (!file) {
        printf("没有找到存档文件\n");
        return false;
    }
    
    printf("正在加载存档...\n");
    
    // 按保存时的顺序读取数据：
    
    // 1. 读取棋盘状态
    if (fread(board, sizeof(int), 10 * 9, file) != 10 * 9) {
        printf("加载失败：棋盘数据损坏\n");
        fclose(file);
        return false;
    }
    
    // 2. 读取游戏记录
    if (fread(&current_game, sizeof(GameRecord), 1, file) != 1) {
        printf("加载失败：游戏记录损坏\n");
        fclose(file);
        return false;
    }
    
    // 3. 读取游戏状态
    fread(&is_red_turn, sizeof(bool), 1, file);
    fread(&move_step, sizeof(int), 1, file);
    
    // 4. 读取棋子选择状态
    fread(&is_piece_selected, sizeof(bool), 1, file);
    fread(&selected_x, sizeof(int), 1, file);
    fread(&selected_y, sizeof(int), 1, file);
    fread(&selected_piece, sizeof(int), 1, file);
    
    // 5. 新增：读取游戏胜负状态
    fread(&is_shuai_live, sizeof(bool), 1, file);
    fread(&is_jiang_live, sizeof(bool), 1, file);
    fread(&redFlyToWin, sizeof(bool), 1, file);
    fread(&blackFlyToWin, sizeof(bool), 1, file);
    
    fclose(file);
    
    // 显示加载信息
    printf("存档加载成功！\n");
    printf("对局：%s vs %s\n", 
           current_game.player_red, current_game.player_black);
    printf("已走：%d 步\n", current_game.move_count);
    printf("轮到：%s\n", is_red_turn ? "红方" : "黑方");
    printf("红帅存活：%s，黑将存活：%s\n", 
           is_shuai_live ? "是" : "否", 
           is_jiang_live ? "是" : "否");
    
    return true;
}