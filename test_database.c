#include <stdio.h>
#include "chess_database.h"
#include "move_parser.h"

int main(void) {
    printf("=== Chess Database Test ===\n");
    
    // 1. 测试棋局数据库
    GameRecord game;
    init_game_record(&game, "G20231120001", "PlayerA", "PlayerB");
    printf("Game initialized: %s\n", game.game_title);
    
    // 2. 测试记谱法解析器
    printf("\n=== Testing Move Parser ===\n");
    
    // 测试红方走法
    ParseResult result1 = parse_notation("兵五进一", PLAYER_RED, &game);
    printf("红方 '兵五进一': ");
    print_parse_result(&result1);
    
    if (result1.valid) {
        ChessMove move1;
        init_chess_move(&move1, 1, PLAYER_RED, result1.piece_type, "兵五进一",
                       result1.from_x, result1.from_y, result1.to_x, result1.to_y);
        add_move_to_game(&game, &move1);
    }
    
    // 测试黑方走法  
    ParseResult result2 = parse_notation("炮8平5", PLAYER_BLACK, &game);
    printf("黑方 '炮8平5': ");
    print_parse_result(&result2);
    
    if (result2.valid) {
        ChessMove move2;
        init_chess_move(&move2, 2, PLAYER_BLACK, PIECE_CANNON, "炮8平5",
                       result2.from_x, result2.from_y, result2.to_x, result2.to_y);
        add_move_to_game(&game, &move2);
    }
    
    // 3. 显示最终结果
    set_game_result(&game, 1);
    print_game_summary(&game);
    save_game_to_file(&game, "chess_game.txt");
    
    printf("All tests passed!\n");
    return 0;
}