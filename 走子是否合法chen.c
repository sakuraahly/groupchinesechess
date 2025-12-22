#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 棋盘常量定义
#define BOARD_ROWS 10    // 棋盘行数
#define BOARD_COLS 9     // 棋盘列数
#define EMPTY 0          // 空位置标记

// 棋子颜色定义（十位）
#define RED 1            // 红方
#define BLACK 2          // 黑方

// 棋子种类定义（个位）
#define SOLDIER 1        // 兵/卒
#define CHARIOT 2        // 车
#define CANNON 3         // 炮
#define HORSE 4          // 马
#define ELEPHANT 5       // 象/相
#define ADVISOR 6        // 士/仕
#define GENERAL 7        // 将/帅

// 使用两位数定义棋子：十位代表颜色，个位代表种类
#define RED_SOLDIER     (RED * 10 + SOLDIER)     // 红兵 = 11
#define BLACK_SOLDIER   (BLACK * 10 + SOLDIER)   // 黑卒 = 21
#define RED_CHARIOT     (RED * 10 + CHARIOT)     // 红车 = 12
#define BLACK_CHARIOT   (BLACK * 10 + CHARIOT)   // 黑车 = 22
#define RED_CANNON      (RED * 10 + CANNON)      // 红炮 = 13
#define BLACK_CANNON    (BLACK * 10 + CANNON)    // 黑炮 = 23

// 为未来扩展预留的其他棋子定义
#define RED_HORSE       (RED * 10 + HORSE)       // 红马 = 14
#define BLACK_HORSE     (BLACK * 10 + HORSE)     // 黑马 = 24
#define RED_ELEPHANT    (RED * 10 + ELEPHANT)    // 红相 = 15
#define BLACK_ELEPHANT  (BLACK * 10 + ELEPHANT)  // 黑象 = 25
#define RED_ADVISOR     (RED * 10 + ADVISOR)     // 红仕 = 16
#define BLACK_ADVISOR   (BLACK * 10 + ADVISOR)   // 黑士 = 26
#define RED_GENERAL     (RED * 10 + GENERAL)     // 红帅 = 17
#define BLACK_GENERAL   (BLACK * 10 + GENERAL)   // 黑将 = 27

// 河流边界定义
#define RIVER_RED 5      // 红方过河边界
#define RIVER_BLACK 4    // 黑方过河边界

// 全局棋盘数组
int board[BOARD_ROWS][BOARD_COLS] = {0};

// 工具函数：获取棋子的颜色（十位数）
int get_piece_color(int piece) {
    return piece / 10;
}

// 工具函数：获取棋子的种类（个位数）
int get_piece_type(int piece) {
    return piece % 10;
}

// 工具函数：判断棋子是否为红色
bool is_red_piece(int piece) {
    return get_piece_color(piece) == RED;
}

// 工具函数：判断棋子是否为黑色
bool is_black_piece(int piece) {
    return get_piece_color(piece) == BLACK;
}

/**
 * 初始化棋盘，设置棋子的初始位置
 */
void init_board() {
    // 初始化红兵位置（位于棋盘第6行）
    board[6][0] = RED_SOLDIER;  // 红兵在第一列
    board[6][2] = RED_SOLDIER;  // 红兵在第三列
    board[6][4] = RED_SOLDIER;  // 红兵在第五列
    board[6][6] = RED_SOLDIER;  // 红兵在第七列
    board[6][8] = RED_SOLDIER;  // 红兵在第九列
    
    // 初始化黑卒位置（位于棋盘第3行）
    board[3][0] = BLACK_SOLDIER;  // 黑卒在第一列
    board[3][2] = BLACK_SOLDIER;  // 黑卒在第三列
    board[3][4] = BLACK_SOLDIER;  // 黑卒在第五列
    board[3][6] = BLACK_SOLDIER;  // 黑卒在第七列
    board[3][8] = BLACK_SOLDIER;  // 黑卒在第九列
    
    // 初始化车的位置
    board[9][0] = RED_CHARIOT;   // 红车在左下角
    board[9][8] = RED_CHARIOT;   // 红车在右下角
    board[0][0] = BLACK_CHARIOT; // 黑车在左上角
    board[0][8] = BLACK_CHARIOT; // 黑车在右上角
    
    // 初始化炮的位置
    board[7][1] = RED_CANNON;    // 红炮在第七行第二列
    board[7][7] = RED_CANNON;    // 红炮在第七行第八列
    board[2][1] = BLACK_CANNON;  // 黑炮在第二行第二列
    board[2][7] = BLACK_CANNON;  // 黑炮在第二行第八列
}

/**
 * 显示当前棋盘状态
 */
void display_board() {
    printf("中国象棋棋盘:\n");
    printf("  0 1 2 3 4 5 6 7 8\n");
    for (int i = 0; i < BOARD_ROWS; i++) {
        printf("%d ", i);
        for (int j = 0; j < BOARD_COLS; j++) {
            int piece = board[i][j];
            int color = get_piece_color(piece);
            int type = get_piece_type(piece);
            
            // 根据棋子类型和颜色显示不同的字符
            if (piece == EMPTY) {
                printf(". ");
            } else if (color == RED) {
                // 红方棋子
                switch(type) {
                    case SOLDIER: printf("兵"); break;
                    case CHARIOT: printf("車"); break;
                    case CANNON:  printf("炮"); break;
                    case HORSE:   printf("马"); break;
                    case ELEPHANT:printf("相"); break;
                    case ADVISOR: printf("仕"); break;
                    case GENERAL: printf("帅"); break;
                    default:      printf("? "); break;
                }
            } else if (color == BLACK) {
                // 黑方棋子
                switch(type) {
                    case SOLDIER: printf("卒"); break;
                    case CHARIOT: printf("车"); break;
                    case CANNON:  printf("砲"); break;
                    case HORSE:   printf("馬"); break;
                    case ELEPHANT:printf("象"); break;
                    case ADVISOR: printf("士"); break;
                    case GENERAL: printf("将"); break;
                    default:      printf("? "); break;
                }
            } else {
                printf("? ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * 检查位置是否在棋盘范围内
 */
bool is_valid_position(int x, int y) {
    return x >= 0 && x < BOARD_ROWS && y >= 0 && y < BOARD_COLS;
}

/**
 * 判断两个棋子是否为同一方
 */
bool is_same_side(int piece1, int piece2) {
    if (piece1 == EMPTY || piece2 == EMPTY) return false;
    
    // 使用工具函数获取颜色进行比较
    return get_piece_color(piece1) == get_piece_color(piece2);
}

/**
 * 计算两个位置之间的棋子数量
 */
int count_pieces_between(int x1, int y1, int x2, int y2) {
    int count = 0;
    
    if (x1 == x2) { // 水平移动
        int start = (y1 < y2) ? y1 : y2;
        int end = (y1 < y2) ? y2 : y1;
        for (int y = start + 1; y < end; y++) {
            if (board[x1][y] != EMPTY) {
                count++;
            }
        }
    } else if (y1 == y2) { // 垂直移动
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

/**
 * 判断兵/卒的移动是否合法
 */
bool is_soldier_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
    // 检查目标位置是否有己方棋子
    if (board[to_x][to_y] != EMPTY && is_same_side(piece, board[to_x][to_y])) {
        return false;
    }
    
    int color = get_piece_color(piece);
    
    // 红兵移动规则
    if (color == RED) {
        // 未过河（红兵在己方半场）
        if (from_x > RIVER_RED) {
            // 只能向前移动一步
            if (to_x == from_x - 1 && to_y == from_y) {
                return true;
            }
        } 
        // 已过河（红兵在对方半场）
        else {
            // 可以向前、向左、向右移动一步
            if ((to_x == from_x - 1 && to_y == from_y) ||  // 向前
                (to_x == from_x && to_y == from_y - 1) ||  // 向左
                (to_x == from_x && to_y == from_y + 1)) {  // 向右
                return true;
            }
        }
    }
    // 黑卒移动规则
    else if (color == BLACK) {
        // 未过河（黑卒在己方半场）
        if (from_x < RIVER_BLACK) {
            // 只能向前移动一步
            if (to_x == from_x + 1 && to_y == from_y) {
                return true;
            }
        } 
        // 已过河（黑卒在对方半场）
        else {
            // 可以向前、向左、向右移动一步
            if ((to_x == from_x + 1 && to_y == from_y) ||  // 向前
                (to_x == from_x && to_y == from_y - 1) ||  // 向左
                (to_x == from_x && to_y == from_y + 1)) {  // 向右
                return true;
            }
        }
    }
    
    return false;
}

/**
 * 判断车的移动是否合法
 */
bool is_chariot_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
    // 车只能直线移动（水平或垂直）
    if (from_x != to_x && from_y != to_y) {
        return false;
    }
    
    
    
    // 检查移动路径上是否有其他棋子
    int piece_count = count_pieces_between(from_x, from_y, to_x, to_y);
    if (piece_count != 0) {
        return false;
    }
    
    return true;
}

/**
 * 判断炮的移动是否合法
 */
bool is_cannon_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
    // 炮只能直线移动（水平或垂直）
    if (from_x != to_x && from_y != to_y) {
        return false;
    }
    
    // 计算移动路径上的棋子数量
    int piece_count = count_pieces_between(from_x, from_y, to_x, to_y);
    
    // 目标位置为空：移动时中间不能有棋子
    if (board[to_x][to_y] == EMPTY) {
        if (piece_count != 0) {
            return false;
        }
    } 
    // 目标位置有棋子：吃子时需要恰好有一个中间棋子作为"炮架"
    else {
        // 不能吃己方棋子
        if (is_same_side(piece, board[to_x][to_y])) {
            return false;
        }
        
        // 吃子时必须恰好有一个中间棋子
        if (piece_count != 1) {
            return false;
        }
    }
    
    return true;
}

/**
 * 判断走子是否合法（总验证函数）
 */
bool is_move_valid(int from_x, int from_y, int to_x, int to_y) {
    // 检查位置是否在棋盘范围内
    if (!is_valid_position(from_x, from_y) || !is_valid_position(to_x, to_y)) {
        return false;
    }
    
    // 检查起始位置是否有棋子
    int piece = board[from_x][from_y];
    if (piece == EMPTY) {
        return false;
    }
    
    // 根据棋子种类调用相应的移动规则验证函数
    int piece_type = get_piece_type(piece);
    switch(piece_type) {
        case SOLDIER:
            return is_soldier_move_valid(from_x, from_y, to_x, to_y, piece);
        case CHARIOT:
            return is_chariot_move_valid(from_x, from_y, to_x, to_y, piece);
        case CANNON:
            return is_cannon_move_valid(from_x, from_y, to_x, to_y, piece);
        // 为未来扩展预留其他棋子类型
        default:
            return false;
    }
}

/**
 * 执行走子操作
 */
bool make_move(int from_x, int from_y, int to_x, int to_y) {
    if (is_move_valid(from_x, from_y, to_x, to_y)) {
        // 执行走子：将棋子移动到目标位置，清空起始位置
        board[to_x][to_y] = board[from_x][from_y];
        board[from_x][from_y] = EMPTY;
        return true;
    } else {
        printf("错误：移动不合法！\n");
        return false;
    }
}

