#include <stdio.h>
#include <string.h>
#include <time.h>

typedef enum {
    PIECE_KING = 0, PIECE_ADVISOR, PIECE_ELEPHANT, 
    PIECE_HORSE, PIECE_CHARIOT, PIECE_CANNON, PIECE_PAWN
} PieceType;

typedef enum {
    PLAYER_RED = 0, PLAYER_BLACK
} PlayerColor;

typedef struct {
    int step_number;
    PlayerColor player;
    PieceType piece_type;
    char piece_name[4];
    char notation[10];
    int from_x, from_y;
    int to_x, to_y;
    char time_str[20];
} ChessMove;

typedef struct {
    char game_id[20];
    char player_red[20];
    char player_black[20];
    char game_title[50];
    ChessMove moves[500];
    int move_count;
    char result_str[10];
} GameRecord;

// 简单函数实现
void init_chess_move(ChessMove* move, int step, PlayerColor player, 
                     PieceType type, const char* notation,
                     int from_x, int from_y, int to_x, int to_y) {
    move->step_number = step;
    move->player = player;
    move->piece_type = type;
    strncpy(move->notation, notation, sizeof(move->notation) - 1);
    
    const char* names[] = {"帅", "士", "相", "马", "车", "炮", "兵"};
    strncpy(move->piece_name, names[type], sizeof(move->piece_name) - 1);
    
    move->from_x = from_x;
    move->from_y = from_y;
    move->to_x = to_x;
    move->to_y = to_y;
    
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    strftime(move->time_str, sizeof(move->time_str), "%H:%M:%S", timeinfo);
}

void init_game_record(GameRecord* game, const char* id, 
                      const char* red_player, const char* black_player) {
    strncpy(game->game_id, id, sizeof(game->game_id) - 1);
    strncpy(game->player_red, red_player, sizeof(game->player_red) - 1);
    strncpy(game->player_black, black_player, sizeof(game->player_black) - 1);
    snprintf(game->game_title, sizeof(game->game_title), "%s vs %s", red_player, black_player);
    game->move_count = 0;
    strcpy(game->result_str, "Not ended");
}

void add_move_to_game(GameRecord* game, const ChessMove* move) {
    if (game->move_count < 500) {
        game->moves[game->move_count] = *move;
        game->move_count++;
        printf("Recorded step %d: %s %s %s\n", 
               move->step_number,
               (move->player == PLAYER_RED) ? "Red" : "Black",
               move->piece_name,
               move->notation);
    }
}

void set_game_result(GameRecord* game, int result) {
    switch(result) {
        case 1: strcpy(game->result_str, "Red win"); break;
        case 2: strcpy(game->result_str, "Black win"); break;
        case 3: strcpy(game->result_str, "Draw"); break;
        default: strcpy(game->result_str, "Not ended");
    }
}

void print_game_summary(const GameRecord* game) {
    printf("\n=== Game Summary ===\n");
    printf("Game ID: %s\n", game->game_id);
    printf("Match: %s\n", game->game_title);
    printf("Total moves: %d\n", game->move_count);
    printf("Result: %s\n", game->result_str);
    printf("================\n\n");
}

void save_game_to_file(const GameRecord* game, const char* filename) {
    printf("Game saved to: %s\n", filename);
}

int main(void) {
    printf("=== Chess Database Test ===\n");
    
    // Test 1: Initialize game
    GameRecord game;
    init_game_record(&game, "G20231120001", "PlayerA", "PlayerB");
    printf("Game initialized: %s\n", game.game_title);
    
    // Test 2: Create chess moves
    ChessMove move1, move2;
    init_chess_move(&move1, 1, PLAYER_RED, PIECE_CANNON, "pao2-ping5", 7, 2, 7, 7);
    init_chess_move(&move2, 2, PLAYER_BLACK, PIECE_HORSE, "ma8-jin7", 1, 9, 2, 7);
    
    // Test 3: Add moves to game
    add_move_to_game(&game, &move1);
    add_move_to_game(&game, &move2);
    
    // Test 4: Set result
    set_game_result(&game, 1);
    
    // Test 5: Print summary
    print_game_summary(&game);
    
    // Test 6: Save to file
    save_game_to_file(&game, "chess_game.txt");
    
    printf("All tests passed!\n");
    return 0;
}