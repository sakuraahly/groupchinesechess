#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BOARD_ROWS 10
#define BOARD_COLS 9
#define EMPTY 0


#define RED_SOLDIER 1    // 红兵
#define BLACK_SOLDIER 2  // 黑卒
#define RED_CHARIOT 3    // 红车
#define BLACK_CHARIOT 4  // 黑车
#define RED_CANNON 5     // 红炮
#define BLACK_CANNON 6   // 黑炮


#define RIVER_RED 5      // 红方过河边界
#define RIVER_BLACK 4    // 黑方过河边界


int board[BOARD_ROWS][BOARD_COLS] = {0};


void init_board() {
    // 红兵 
    board[6][0] = RED_SOLDIER;
    board[6][2] = RED_SOLDIER;
    board[6][4] = RED_SOLDIER;
    board[6][6] = RED_SOLDIER;
    board[6][8] = RED_SOLDIER;
    
    // 黑兵
    board[3][0] = BLACK_SOLDIER;
    board[3][2] = BLACK_SOLDIER;
    board[3][4] = BLACK_SOLDIER;
    board[3][6] = BLACK_SOLDIER;
    board[3][8] = BLACK_SOLDIER;
    
    // 车
    board[9][0] = RED_CHARIOT;
    board[9][8] = RED_CHARIOT;
    board[0][0] = BLACK_CHARIOT;
    board[0][8] = BLACK_CHARIOT;
    
    // 炮
    board[7][1] = RED_CANNON;
    board[7][7] = RED_CANNON;
    board[2][1] = BLACK_CANNON;
    board[2][7] = BLACK_CANNON;
}


void display_board() {
    printf("棋盘:\n");
    printf("  0 1 2 3 4 5 6 7 8\n");
    for (int i = 0; i < BOARD_ROWS; i++) {
        printf("%d ", i);
        for (int j = 0; j < BOARD_COLS; j++) {
            switch(board[i][j]) {
                case EMPTY:
                    printf(". ");
                    break;
                case RED_SOLDIER:
                    printf("兵");
                    break;
                case BLACK_SOLDIER:
                    printf("卒");
                    break;
                case RED_CHARIOT:
                    printf("車");
                    break;
                case BLACK_CHARIOT:
                    printf("车");
                    break;
                case RED_CANNON:
                    printf("炮");
                    break;
                case BLACK_CANNON:
                    printf("砲");
                    break;
                default:
                    printf("? ");
            }
        }
        printf("\n");
    }
    printf("\n");
}


bool is_valid_position(int x, int y) {
    return x >= 0 && x < BOARD_ROWS && y >= 0 && y < BOARD_COLS;
}


bool is_same_side(int piece1, int piece2) {
    if (piece1 == EMPTY || piece2 == EMPTY) return false;
    
    
    bool is_red1 = (piece1 == RED_SOLDIER || piece1 == RED_CHARIOT || piece1 == RED_CANNON);
    bool is_red2 = (piece2 == RED_SOLDIER || piece2 == RED_CHARIOT || piece2 == RED_CANNON);
    
    return is_red1 == is_red2;
}


int count_pieces_between(int x1, int y1, int x2, int y2) {
    int count = 0;
    
    if (x1 == x2) { 
        int start = (y1 < y2) ? y1 : y2;
        int end = (y1 < y2) ? y2 : y1;
        for (int y = start + 1; y < end; y++) {
            if (board[x1][y] != EMPTY) {
                count++;
            }
        }
    } else if (y1 == y2) { 
        int start = (x1 < x2) ? x1 : x2;
        int end = (x1 < x2) ? x2 : x1;
        for (int x = start + 1; x < end; x++) {
            if (board[x][y1] != EMPTY) {
                count++;
            }
        }
    }
    
    return count;
}

//兵 
bool is_soldier_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
    
    if (board[to_x][to_y] != EMPTY && is_same_side(piece, board[to_x][to_y])) {
        return false;
    }
    
    // 红兵移动规则
    if (piece == RED_SOLDIER) {
        // 未过河
        if (from_x > RIVER_RED) {
            
            if (to_x == from_x - 1 && to_y == from_y) {
                return true;
            }
        } 
        // 已过河
        else {
            
            if ((to_x == from_x - 1 && to_y == from_y) ||  // 向前
                (to_x == from_x && to_y == from_y - 1) ||  // 向左
                (to_x == from_x && to_y == from_y + 1)) {  // 向右
                return true;
            }
        }
    }
    // 黑卒移动规则
    else if (piece == BLACK_SOLDIER) {
        // 未过河
        if (from_x < RIVER_BLACK) {
            
            if (to_x == from_x + 1 && to_y == from_y) {
                return true;
            }
        } 
        // 已过河
        else {
            
            if ((to_x == from_x + 1 && to_y == from_y) ||  // 向前
                (to_x == from_x && to_y == from_y - 1) ||  // 向左
                (to_x == from_x && to_y == from_y + 1)) {  // 向右
                return true;
            }
        }
    }
    
    return false;
}

//车 
bool is_chariot_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
    
    if (from_x != to_x && from_y != to_y) {
        return false;
    }
    
    
    if (board[to_x][to_y] != EMPTY && is_same_side(piece, board[to_x][to_y])) {
        return false;
    }
    
    //中间没棋子 
    int piece_count = count_pieces_between(from_x, from_y, to_x, to_y);
    if (piece_count != 0) {
        return false;
    }
    
    return true;
}

//炮 
bool is_cannon_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
    
    if (from_x != to_x && from_y != to_y) {
        return false;
    }
    
  
    int piece_count = count_pieces_between(from_x, from_y, to_x, to_y);
    
    // 目标位置空：中间无棋子
    if (board[to_x][to_y] == EMPTY) {
        if (piece_count != 0) {
            return false;
        }
    } 
    // 目标位置有棋子
    else {
        // 不能吃同方的棋子
        if (is_same_side(piece, board[to_x][to_y])) {
            return false;
        }
        
        // 恰好有一个中间棋子
        if (piece_count != 1) {
            return false;
        }
    }
    
    return true;
}

// 判断走子是否合法
bool is_move_valid(int from_x, int from_y, int to_x, int to_y) {
    // 检查位置是否有效
    if (!is_valid_position(from_x, from_y) || !is_valid_position(to_x, to_y)) {
        return false;
    }
    
    // 检查起始位置是否有棋子
    int piece = board[from_x][from_y];
    if (piece == EMPTY) {
        return false;
    }
    
    // 根据棋子类型调用相应的函数
    switch(piece) {
        case RED_SOLDIER:
        case BLACK_SOLDIER:
            return is_soldier_move_valid(from_x, from_y, to_x, to_y, piece);
            
        case RED_CHARIOT:
        case BLACK_CHARIOT:
            return is_chariot_move_valid(from_x, from_y, to_x, to_y, piece);
            
        case RED_CANNON:
        case BLACK_CANNON:
            return is_cannon_move_valid(from_x, from_y, to_x, to_y, piece);
            
        default:
            return false;
    }
}

// 走子判断 
bool make_move(int from_x, int from_y, int to_x, int to_y) {
    if (is_move_valid(from_x, from_y, to_x, to_y)) {
        board[to_x][to_y] = board[from_x][from_y];
        board[from_x][from_y] = EMPTY;
        return true;
    } else {
        printf("错误\n");
        return false;
    }
}

