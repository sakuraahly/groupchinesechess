#include <stdio.h>
#include "chess_database.h"
#include "move_parser.h"

int main(void) {
    printf("=== 中国象棋棋谱管理系统测试 ===\n");
    printf("坐标系: x(0-9行,从上到下), y(0-8列,从左到右)\n");
    printf("左上角(0,0) = 黑车\n\n");
    
    // 1. 初始化棋局
    GameRecord game;
    init_game_record(&game, "G20231124001", "张三", "李四");
    printf("棋局初始化成功: %s\n", game.game_title);
    
    // 2. 测试基础棋步记录
    printf("\n=== 测试基础棋步记录 ===\n");
    
    ChessMove move1;
    init_chess_move(&move1, 1, RED_BING, "兵五进一", 6, 4, 5, 4);
    add_move_to_game(&game, &move1);
    
    ChessMove move2;
    init_chess_move(&move2, 2, BLACK_PAO, "炮8平5", 2, 1, 2, 4);
    add_move_to_game(&game, &move2);
    
    ChessMove move3;
    init_chess_move(&move3, 3, RED_JU, "车一平二", 7, 8, 7, 7);
    add_move_to_game(&game, &move3);
    
    // 3. 测试解析器
    printf("\n=== 测试记谱法解析器 ===\n");
    
    ParseResult result1 = parse_notation("兵五进一", COLOR_RED, &game);
    printf("红方 '兵五进一': ");
    print_parse_result(&result1);
    
    ParseResult result2 = parse_notation("炮8平5", COLOR_BLACK, &game);
    printf("黑方 '炮8平5': ");
    print_parse_result(&result2);
    
    // 4. 结束棋局
    set_game_result(&game, 1);
    print_game_summary(&game);
    save_game_to_file(&game, "complete_test_game.txt");
    
    printf("=== 测试完成 ===\n");
    return 0;
}