#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>

//自己的的头文件
#include "chess_database.h"
#include "displayinterface.h"
#include "chess_move.h"

//将和帅的位置
place jiang;
place shuai;

// 检查点是否在矩形内
bool pointInRect(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
}

// 辅助函数：获取棋子颜色
int getPieceColor(int piece) {
    if (piece == NONE) return 0;
    return piece / 10;
}

// 辅助函数：获取棋子类型
int getPieceType(int piece) {
    if (piece == NONE) return 0;
    return piece % 10;
}

// ==================== 新增函数实现 ====================

// 屏幕坐标转换为棋盘坐标
bool screenToBoard(int screen_x, int screen_y, int* board_x, int* board_y) {
    // 计算相对于格点原点的坐标
    int relative_x = screen_x - GRID_ORIGIN_X;
    int relative_y = screen_y - GRID_ORIGIN_Y;
    
    // 检查是否在棋盘范围内
    if (relative_x < -PIECE_SIZE/2 || relative_x >= 9*GRID_WIDTH + PIECE_SIZE/2 ||
        relative_y < -PIECE_SIZE/2 || relative_y >= 10*GRID_HEIGHT + PIECE_SIZE/2) {
        return false;
    }
    
    // 计算最近的格点
    *board_y = (relative_x + GRID_WIDTH/2) / GRID_WIDTH;
    *board_x = (relative_y + GRID_HEIGHT/2) / GRID_HEIGHT;
    
    // 检查坐标是否在有效范围内
    if (*board_x < 0 || *board_x >= 10 || *board_y < 0 || *board_y >= 9) {
        return false;
    }
    
    return true;
}

// 检查两个棋子是否同色
bool isSameColor(int piece1, int piece2) {
    if (piece1 == NONE || piece2 == NONE) return false;
    return (getPieceColor(piece1) == getPieceColor(piece2));
}

// 统计两点之间直线上的棋子数量
int countPiecesInLine(int x1, int y1, int x2, int y2) {
    int count = 0;
    
    if (x1 == x2) {
        // 同一行
        int start_y = (y1 < y2) ? y1 : y2;
        int end_y = (y1 < y2) ? y2 : y1;
        
        for (int y = start_y + 1; y < end_y; y++) {
            if (board[x1][y] != NONE) {
                count++;
            }
        }
    } else if (y1 == y2) {
        // 同一列
        int start_x = (x1 < x2) ? x1 : x2;
        int end_x = (x1 < x2) ? x2 : x1;
        
        for (int x = start_x + 1; x < end_x; x++) {
            if (board[x][y1] != NONE) {
                count++;
            }
        }
    }
    
    return count;
}

// 检查移动是否合法（简化版象棋规则,没有将军的判定之类的）
bool isValidMove(int piece_code, int from_x, int from_y, int to_x, int to_y) {
    // 不能原地不动
    if (from_x == to_x && from_y == to_y) return false;
    
    // 不能吃自己的棋子
    if (board[to_x][to_y] != NONE && isSameColor(piece_code, board[to_x][to_y])) {
        return false;
    }
    
    int color = getPieceColor(piece_code);
    int type = getPieceType(piece_code);
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    int abs_dx = abs(dx);
    int abs_dy = abs(dy);
    
    switch(type) {
        case TYPE_JIANG:  // 将/帅
            // 只能在九宫内移动
            if (color == COLOR_RED) {
                if (to_x < 7 || to_x > 9 || to_y < 3 || to_y > 5) return false;
            } else {
                if (to_x < 0 || to_x > 2 || to_y < 3 || to_y > 5) return false;
            }
            // 只能走一步
            if (abs_dx + abs_dy != 1) return false;
            return true;
            
        case TYPE_SHI:    // 士/仕
            // 只能在九宫内斜走
            if (color == COLOR_RED) {
                if (to_x < 7 || to_x > 9 || to_y < 3 || to_y > 5) return false;
            } else {
                if (to_x < 0 || to_x > 2 || to_y < 3 || to_y > 5) return false;
            }
            // 只能斜走一步
            if (abs_dx != 1 || abs_dy != 1) return false;
            return true;
            
        case TYPE_XIANG:  // 象/相
            // 不能过河
            if (color == COLOR_RED && to_x < 5) return false;
            if (color == COLOR_BLACK && to_x > 4) return false;
            // 田字走法
            if (abs_dx != 2 || abs_dy != 2) return false;
            // 检查象眼是否被塞
            int block_x = from_x + dx/2;
            int block_y = from_y + dy/2;
            if (board[block_x][block_y] != NONE) return false;
            return true;
            
        case TYPE_MA:     // 马
            // 日字走法
            if (!((abs_dx == 2 && abs_dy == 1) || (abs_dx == 1 && abs_dy == 2))) {
                return false;
            }
            // 检查蹩马腿
            if (abs_dx == 2) {
                int block_x = from_x + dx/2;
                if (board[block_x][from_y] != NONE) return false;
            } else {
                int block_y = from_y + dy/2;
                if (board[from_x][block_y] != NONE) return false;
            }
            return true;
            
        case TYPE_JU:     // 车
            // 直线移动
            if (from_x != to_x && from_y != to_y) return false;
            // 路径上不能有其他棋子
            if (countPiecesInLine(from_x, from_y, to_x, to_y) > 0) return false;
            return true;
            
        case TYPE_PAO:    // 炮
            // 直线移动
            if (from_x != to_x && from_y != to_y) return false;
            int pieces_count = countPiecesInLine(from_x, from_y, to_x, to_y);
            // 吃子时需要隔一个棋子
            if (board[to_x][to_y] != NONE) {
                return pieces_count == 1;
            } else {
                return pieces_count == 0;
            }
            
        case TYPE_BING:   // 兵/卒
            // 红方向上移动，黑方向下移动
            if (color == COLOR_RED) {
                if (dx > 0) return false;  // 红兵不能后退
                if (from_x > 4 && abs_dx > 1) return false;  // 过河前只能走一步
            } else {
                if (dx < 0) return false;  // 黑卒不能后退
                if (from_x < 5 && abs_dx > 1) return false;  // 过河前只能走一步
            }
            // 只能走一步
            if (abs_dx + abs_dy != 1) return false;
            // 不能横向移动（除非过河）
            if (abs_dy > 0) {
                if (color == COLOR_RED && from_x >= 5) return false;  // 红兵没过河
                if (color == COLOR_BLACK && from_x <= 4) return false; // 黑卒没过河
            }
            return true;
            
        default:
            return false;
    }
}

//寻找将的位置 -hu 12.22
place* find_jiang(place* jiang){
for(int row=0; row < 3;row++){//来找棋盘上方的将(黑方)
            for(int col=3;col<6;col++){
                if (board[row][col] == 21){
                    jiang->x = row;
                    jiang->y = col;
                     return jiang;
                }
        }
    }
       
}
//寻找帅的位置 -hu 12.22
place* find_shuai(place* shuai){
for(int row=7; row < 10;row++){//来找棋盘下方的帅(红方)
            for(int col=3;col<6;col++){
                    if (board[row][col] == 11){
                            shuai->x = row;
                            shuai->y = col;
                            return shuai;

                    }
                }
            }
        }        

//判断是否将军,将军就播放音效 -hu 12.22
void is_jiangToDeath(place jiang){
    for(int row=0; row < 10;row++){
            for(int col=0;col<9;col++){
                if(board != NONE){
                    int piececode = board[row][col];
                    if(isValidMove(piececode, row, col, jiang.x, jiang.y)){
                        //播放将军音频
                        Mix_PlayChannel(-1,jiangjun, 0);
                    }
                } 
            }
        }
}
//判断是否将军,将军就播放音效 -hu 12.22
void is_shuaiToDeath(place shuai){
        for(int row=0; row < 10;row++){
            for(int col=0;col<9;col++){
                 if(board != NONE){
                    int piececode = board[row][col];
                    if(isValidMove(piececode, row, col, jiang.x, jiang.y)){
                        //播放将军音频
                        Mix_PlayChannel(-1,jiangjun, 0);
                    }
                } 
            }
        }
    }
// ============================== 


// 移动棋子
bool movePiece(int from_x, int from_y, int to_x, int to_y) {
    int piece = board[from_x][from_y];
    
    if (piece == NONE) return false;
    
    // 检查移动是否合法
    if (!isValidMove(piece, from_x, from_y, to_x, to_y)) {
        //printf("非法移动！\n");
        return false;
    }
    
    // 记录吃子信息
    int captured_piece = board[to_x][to_y];
    
    // 执行移动
    board[to_x][to_y] = piece;
    board[from_x][from_y] = NONE;

    //这里有一个小bug,在displayinterface.h的外部变量只能让src/chess_move.c不报错,但是会导致编译失败 -hu 12.21
    //解决了 初始化函数作用域的坑 -hu 12.22
    //Mix_Chunk* eat = Mix_LoadWAV("res/music/eat.mp3");

    Mix_PlayChannel(-1, eat, 0);  //吃子和移动的音效

    //下面这个是原来的解决方案,但是采用多频道播放音效之后就不用这个了 -hu 12.21
    //is_music_playing = false;
    
    // 生成记谱法
    generateNotation(&current_move, from_x, from_y, to_x, to_y, piece);
    
    // 初始化棋步记录
    init_chess_move(&current_move, move_step, piece, 
                   current_move.notation, from_x, from_y, to_x, to_y);
    
    // 计算思考时间
    if (move_start_time > 0) {
        current_move.thinking_time = (int)(time(NULL) - move_start_time);
    }
    
    // 添加到棋局记录
    add_move_to_game(&current_game, &current_move);
    
    // 更新状态
    move_step++;
    is_red_turn = !is_red_turn;
    move_start_time = time(NULL);
    
    //printf("移动成功！第%d步：%s %s\n", current_move.step_number, current_move.piece_name, current_move.notation);
    
    //我觉得有可能在这里进行将军的判定.  -hu 12.21
    //实现了 -hu 12.22
       static place jiang_place={0,4};
       static place shuai_place={9,4};

         shuai_place = *find_shuai(&shuai_place);
         jiang_place = *find_jiang(&jiang_place);

         is_jiangToDeath(jiang_place);
        is_shuaiToDeath(shuai_place);
    return true;
}

// 悔棋功能
void revokeLastMove() {
    if (current_game.move_count == 0) {
        //printf("没有棋步可以悔棋！\n");
        return;
    }
    
    ChessMove* last_move = &current_game.moves[current_game.move_count - 1];
    
    // 恢复棋盘状态
    board[last_move->from_x][last_move->from_y] = last_move->piece_code;
    board[last_move->to_x][last_move->to_y] = NONE;
    
    // 更新棋局记录
    current_game.move_count--;
    
    // 更新游戏状态
    move_step--;
    is_red_turn = !is_red_turn;
    
    //printf("悔棋成功！恢复第%d步\n", last_move->step_number);
}

// 处理棋盘点击
void handleBoardClick(int board_x, int board_y) {
    int clicked_piece = board[board_x][board_y];
    
    if (!is_piece_selected) {
        // 第一次点击：选择棋子
        if (clicked_piece == NONE) {
            //printf("点击了空位置\n");
            return;
        }
        
        // 检查是否轮到该棋子走
        int piece_color = getPieceColor(clicked_piece);
        if ((is_red_turn && piece_color != COLOR_RED) ||
            (!is_red_turn && piece_color != COLOR_BLACK)) {
            //printf("现在轮到%s走棋！\n", is_red_turn ? "红方" : "黑方");
            return;
        }
        
        // 选择棋子
        is_piece_selected = true;

        //Mix_Music* choseChess = Mix_LoadMUS("res/music/chose.mp3");
        Mix_PlayChannel(-1,choseChess, 0);
        selected_x = board_x;
        selected_y = board_y;
        selected_piece = clicked_piece;
        
    } else {
        // 第二次点击：移动棋子或取消选择
        if (board_x == selected_x && board_y == selected_y) {
            // 点击同一位置：取消选择
            is_piece_selected = false;
            //printf("取消选择\n");
        } else {
            // 尝试移动棋子
            if (movePiece(selected_x, selected_y, board_x, board_y)) {
                is_piece_selected = false;
            } else {
                //printf("移动失败，请重新选择目标位置\n");
                return;
            }
        }
    }
}
