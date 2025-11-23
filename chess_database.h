#ifndef CHESS_DATABASE_H
#define CHESS_DATABASE_H

#include <time.h>

// 棋子类型枚举
typedef enum {
    PIECE_KING = 0,     // 帅/将
    PIECE_ADVISOR,      // 士/仕  
    PIECE_ELEPHANT,     // 相/象
    PIECE_HORSE,        // 马
    PIECE_CHARIOT,      // 车
    PIECE_CANNON,       // 炮
    PIECE_PAWN          // 兵/卒
} PieceType;

// 玩家阵营枚举
typedef enum {
    PLAYER_RED = 0,     // 红方
    PLAYER_BLACK        // 黑方
} PlayerColor;

// 单步棋记录结构体
typedef struct {
    int step_number;            // 步数编号 (1, 2, 3...)
    PlayerColor player;         // 走棋玩家
    PieceType piece_type;       // 棋子类型
    char piece_name[4];         // 棋子中文名
    
    // 记谱法信息
    char notation[10];          // 原始记谱: "兵五进一"
    
    // 坐标信息 (0-8, 0-9)
    int from_x, from_y;         // 起始位置
    int to_x, to_y;             // 目标位置
    
    // 时间信息  
    time_t timestamp;           // 时间戳 (Unix时间)
    char time_str[20];          // 格式化的时间字符串
    
    int thinking_time;          // 思考时间(秒)
} ChessMove;

// 棋局记录结构体
typedef struct {
    char game_id[20];           // 棋局ID: "G20231120001"
    char player_red[20];        // 红方玩家名
    char player_black[20];      // 黑方玩家名
    char game_title[50];        // 棋局标题
    
    ChessMove moves[500];       // 棋步记录数组
    int move_count;             // 总步数
    
    // 棋局结果
    int result;                 // 0=未结束, 1=红胜, 2=黑胜, 3=平局
    char result_str[10];        // 结果字符串: "红胜", "黑胜", "平局"
    
    time_t start_time;          // 开始时间
    time_t end_time;            // 结束时间
    
    // 统计信息
    int total_red_thinking;     // 红方总思考时间
    int total_black_thinking;   // 黑方总思考时间
} GameRecord;

// 函数声明
void init_chess_move(ChessMove* move, int step, PlayerColor player, 
                     PieceType type, const char* notation,
                     int from_x, int from_y, int to_x, int to_y);

void init_game_record(GameRecord* game, const char* id, 
                      const char* red_player, const char* black_player);

void add_move_to_game(GameRecord* game, const ChessMove* move);

void set_game_result(GameRecord* game, int result);

// 工具函数
const char* get_piece_name_cn(PieceType type);
const char* get_player_name_cn(PlayerColor player);
void get_current_timestamp(char* buffer, int size);

#endif // CHESS_DATABASE_H
// 添加缺失的函数声明
void print_game_summary(const GameRecord* game);
void save_game_to_file(const GameRecord* game, const char* filename);
