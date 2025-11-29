

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 棋盘常量定义
#define BOARD_ROWS 10    // 棋盘行数
#define BOARD_COLS 9     // 棋盘列数
#define EMPTY 0          // 空位置标记

// 棋子颜色定义
#define RED 1            // 红方
#define BLACK 2          // 黑方

// 棋子种类定义
#define TYPE_JIANG  1         // 将/帅
#define TYPE_SHI    2         // 士/仕  
#define TYPE_XIANG  3         // 象/相
#define TYPE_MA     4         // 马
#define TYPE_JU     5         // 车
#define TYPE_PAO    6         // 炮
#define TYPE_BING   7         // 兵/卒

// 红方棋子
#define RED_JIANG   11   // 红帅
#define RED_SHI     12   // 红仕
#define RED_XIANG   13   // 红相
#define RED_MA      14   // 红马
#define RED_JU      15   // 红车
#define RED_PAO     16   // 红炮
#define RED_BING    17   // 红兵

// 黑方棋子  
#define BLACK_JIANG 21   // 黑将
#define BLACK_SHI   22   // 黑士
#define BLACK_XIANG 23   // 黑象
#define BLACK_MA    24   // 黑马
#define BLACK_JU    25   // 黑车
#define BLACK_PAO   26   // 黑炮
#define BLACK_BING  27   // 黑卒

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
    board[6][0] = RED_BING;  // 红兵在第一列
    board[6][2] = RED_BING;  // 红兵在第三列
    board[6][4] = RED_BING;  // 红兵在第五列
    board[6][6] = RED_BING;  // 红兵在第七列
    board[6][8] = RED_BING;  // 红兵在第九列
    
    // 初始化黑卒位置（位于棋盘第3行）
    board[3][0] = BLACK_BING;  // 黑卒在第一列
    board[3][2] = BLACK_BING;  // 黑卒在第三列
    board[3][4] = BLACK_BING;  // 黑卒在第五列
    board[3][6] = BLACK_BING;  // 黑卒在第七列
    board[3][8] = BLACK_BING;  // 黑卒在第九列
    
    // 初始化车的位置
    board[9][0] = RED_JU;   // 红车在左下角
    board[9][8] = RED_JU;   // 红车在右下角
    board[0][0] = BLACK_JU; // 黑车在左上角
    board[0][8] = BLACK_JU; // 黑车在右上角
    
    // 初始化炮的位置
    board[7][1] = RED_PAO;    // 红炮在第七行第二列
    board[7][7] = RED_PAO;    // 红炮在第七行第八列
    board[2][1] = BLACK_PAO;  // 黑炮在第二行第二列
    board[2][7] = BLACK_PAO;  // 黑炮在第二行第八列
    
    // 初始化将/帅的位置
    board[9][4] = RED_JIANG;   // 红帅在第九行第五列
    board[0][4] = BLACK_JIANG; // 黑将在第一行第五列
    
    // 初始化士/仕的位置
    board[9][3] = RED_SHI;     // 红仕在第九行第四列
    board[9][5] = RED_SHI;     // 红仕在第九行第六列
    board[0][3] = BLACK_SHI;   // 黑士在第一行第四列
    board[0][5] = BLACK_SHI;   // 黑士在第一行第六列
    
    // 初始化象/相的位置
    board[9][2] = RED_XIANG;   // 红相在第九行第三列
    board[9][6] = RED_XIANG;   // 红相在第九行第七列
    board[0][2] = BLACK_XIANG; // 黑象在第一行第三列
    board[0][6] = BLACK_XIANG; // 黑象在第一行第七列
    
    // 初始化马的位置
    board[9][1] = RED_MA;      // 红马在第九行第二列
    board[9][7] = RED_MA;      // 红马在第九行第八列
    board[0][1] = BLACK_MA;    // 黑马在第一行第二列
    board[0][7] = BLACK_MA;    // 黑马在第一行第八列
}

/**
 * 显示当前棋盘状态（改进版，带坐标标注）
 */
void display_board() {
    printf("\n中国象棋棋盘（坐标系统）:\n");
    printf("   ");
    for (int j = 0; j < BOARD_COLS; j++) {
        printf("%d ", j);  // 列坐标
    }
    printf("\n");
    printf("  +");
    for (int j = 0; j < BOARD_COLS; j++) {
        printf("--");
    }
    printf("+\n");
    
    for (int i = 0; i < BOARD_ROWS; i++) {
        printf("%d |", i);  // 行坐标
        for (int j = 0; j < BOARD_COLS; j++) {
            int piece = board[i][j];
            int color = get_piece_color(piece);
            int type = get_piece_type(piece);
            
            // 根据棋子类型和颜色显示不同的字符
            if (piece == EMPTY) {
                printf(" .");
            } else if (color == RED) {
                // 红方棋子
                switch(type) {
                    case TYPE_JIANG:  printf(" 帅"); break;
                    case TYPE_SHI:    printf(" 仕"); break;
                    case TYPE_XIANG:  printf(" 相"); break;
                    case TYPE_MA:     printf(" 马"); break;
                    case TYPE_JU:     printf(" 車"); break;
                    case TYPE_PAO:    printf(" 炮"); break;
                    case TYPE_BING:   printf(" 兵"); break;
                    default:     printf(" ?"); break;
                }
            } else if (color == BLACK) {
                // 黑方棋子
                switch(type) {
                    case TYPE_JIANG:  printf(" 将"); break;
                    case TYPE_SHI:    printf(" 士"); break;
                    case TYPE_XIANG:  printf(" 象"); break;
                    case TYPE_MA:     printf(" 馬"); break;
                    case TYPE_JU:     printf(" 车"); break;
                    case TYPE_PAO:    printf(" 砲"); break;
                    case TYPE_BING:   printf(" 卒"); break;
                    default:     printf(" ?"); break;
                }
            } else {
                printf(" ?");
            }
        }
        printf(" |\n");
    }
    
    printf("  +");
    for (int j = 0; j < BOARD_COLS; j++) {
        printf("--");
    }
    printf("+\n");
    
   
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
bool is_bing_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
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
bool is_ju_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
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
bool is_pao_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
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
 * 判断马的移动是否合法
 */
bool is_ma_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
    // 检查目标位置是否有己方棋子
    if (board[to_x][to_y] != EMPTY && is_same_side(piece, board[to_x][to_y])) {
        return false;
    }
    
    // 马走"日"字，有8个可能的移动方向
    int dx = abs(to_x - from_x);
    int dy = abs(to_y - from_y);
    
    // 马必须走"日"字：一个方向走2格，另一个方向走1格
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) {
        return false;
    }
    
    // 检查"蹩马腿"：马腿位置不能有棋子
    int block_x, block_y;
    
    if (dx == 2) { // 竖向走2格，横向走1格
        block_x = from_x + (to_x - from_x) / 2;
        block_y = from_y;
    } else { // 横向走2格，竖向走1格
        block_x = from_x;
        block_y = from_y + (to_y - from_y) / 2;
    }
    
    // 马腿位置有棋子则不能移动
    if (board[block_x][block_y] != EMPTY) {
        return false;
    }
    
    return true;
}

/**
 * 判断象/相的移动是否合法
 */
bool is_xiang_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
    // 检查目标位置是否有己方棋子
    if (board[to_x][to_y] != EMPTY && is_same_side(piece, board[to_x][to_y])) {
        return false;
    }
    
    int color = get_piece_color(piece);
    
    // 象不能过河
    if (color == RED && to_x > 4) { // 红相不能过河（河界在第4-5行之间）
        return false;
    }
    if (color == BLACK && to_x < 5) { // 黑象不能过河
        return false;
    }
    
    // 象走"田"字，必须走对角线2格
    int dx = abs(to_x - from_x);
    int dy = abs(to_y - from_y);
    
    if (dx != 2 || dy != 2) {
        return false;
    }
    
    // 检查"塞象眼"：象眼位置不能有棋子
    int block_x = from_x + (to_x - from_x) / 2;
    int block_y = from_y + (to_y - from_y) / 2;
    
    if (board[block_x][block_y] != EMPTY) {
        return false;
    }
    
    return true;
}

/**
 * 判断士/仕的移动是否合法
 */
bool is_shi_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
    // 检查目标位置是否有己方棋子
    if (board[to_x][to_y] != EMPTY && is_same_side(piece, board[to_x][to_y])) {
        return false;
    }
    
    int color = get_piece_color(piece);
    
    // 士只能在九宫格内移动
    if (color == RED) {
        // 红仕的九宫格：行7-9，列3-5
        if (to_x < 7 || to_x > 9 || to_y < 3 || to_y > 5) {
            return false;
        }
    } else {
        // 黑士的九宫格：行0-2，列3-5
        if (to_x < 0 || to_x > 2 || to_y < 3 || to_y > 5) {
            return false;
        }
    }
    
    // 士只能走斜线，每次移动一格
    int dx = abs(to_x - from_x);
    int dy = abs(to_y - from_y);
    
    if (dx != 1 || dy != 1) {
        return false;
    }
    
    return true;
}

/**
 * 判断将/帅的移动是否合法
 */
bool is_jiang_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) {
    // 检查目标位置是否有己方棋子
    if (board[to_x][to_y] != EMPTY && is_same_side(piece, board[to_x][to_y])) {
        return false;
    }
    
    int color = get_piece_color(piece);
    
    // 将/帅只能在九宫格内移动
    if (color == RED) {
        // 红帅的九宫格：行7-9，列3-5
        if (to_x < 7 || to_x > 9 || to_y < 3 || to_y > 5) {
            return false;
        }
    } else {
        // 黑将的九宫格：行0-2，列3-5
        if (to_x < 0 || to_x > 2 || to_y < 3 || to_y > 5) {
            return false;
        }
    }
    
    // 将/帅只能横着或竖着移动一格
    int dx = abs(to_x - from_x);
    int dy = abs(to_y - from_y);
    
    if (!((dx == 1 && dy == 0) || (dx == 0 && dy == 1))) {
        return false;
    }
    
    // 特殊情况：将帅对面（飞将）
    // 如果目标位置是对方的将/帅，且在同一列且中间无棋子，则允许移动
    int target_piece = board[to_x][to_y];
    if (target_piece != EMPTY) {
        int target_type = get_piece_type(target_piece);
        int target_color = get_piece_color(target_piece);
        
        if (target_type == JIANG && target_color != color) {
            // 检查是否在同一列且中间无棋子
            if (from_y == to_y) {
                int piece_count = count_pieces_between(from_x, from_y, to_x, to_y);
                if (piece_count == 0) {
                    return true; // 允许飞将
                }
            }
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
    
    // 检查是否移动到同一位置
    if (from_x == to_x && from_y == to_y) {
        return false;
    }
    
    // 根据棋子种类调用相应的移动规则验证函数
    int piece_type = get_piece_type(piece);
    switch(piece_type) {
        case TYPE_JIANG:
            return is_jiang_move_valid(from_x, from_y, to_x, to_y, piece);
        case TYPE_SHI:
            return is_shi_move_valid(from_x, from_y, to_x, to_y, piece);
        case TYPE_XIANG:
            return is_xiang_move_valid(from_x, from_y, to_x, to_y, piece);
        case TYPE_MA:
            return is_ma_move_valid(from_x, from_y, to_x, to_y, piece);
        case TYPE_JU:
            return is_ju_move_valid(from_x, from_y, to_x, to_y, piece);
        case TYPE_PAO:
            return is_pao_move_valid(from_x, from_y, to_x, to_y, piece);
        case TYPE_BING:
            return is_bing_move_valid(from_x, from_y, to_x, to_y, piece);
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



