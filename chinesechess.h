#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include<assert.h>
#include<stdbool.h>
#include<time.h>

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
int get_piece_color(int piece);

// 工具函数：获取棋子的种类（个位数）
int get_piece_type(int piece);

// 工具函数：判断棋子是否为红色
bool is_red_piece(int piece) ;

// 工具函数：判断棋子是否为黑色
bool is_black_piece(int piece);

// 初始化棋盘，设置棋子的初始位置
void init_board() ;

//显示当前棋盘状态
void display_board();

// 检查位置是否在棋盘范围内
bool is_valid_position(int x, int y) ;

//判断两个棋子是否为同一方
 bool is_same_side(int piece1, int piece2);

//计算两个位置之间的棋子数量
 int count_pieces_between(int x1, int y1, int x2, int y2);

//判断兵/卒的移动是否合法
bool is_soldier_move_valid(int from_x, int from_y, int to_x, int to_y, int piece);

//判断车的移动是否合法
bool is_chariot_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) ;

// 判断炮的移动是否合法
bool is_cannon_move_valid(int from_x, int from_y, int to_x, int to_y, int piece);

//判断走子是否合法（总验证函数）
bool is_move_valid(int from_x, int from_y, int to_x, int to_y) ;

//执行走子操作
bool make_move(int from_x, int from_y, int to_x, int to_y) ;








/*huixui代码*/
// 棋盘状态结构体
typedef struct chess{
    int playboard[BOARD_ROWS][BOARD_COLS];
    struct chess* prev;
} BoardState;

//记录棋盘
void Copyboard(int BOARD2[BOARD_ROWS][BOARD_COLS],int BOARD1[BOARD_ROWS][BOARD_COLS]);

// 初始化棋盘，设置棋子的初始位置(未写完)
void initplayboard(int BOARD[BOARD_ROWS][BOARD_COLS]);

//创建节点
BoardState* buynode(int BOARD[BOARD_ROWS][BOARD_COLS]);

//走棋后链表更新
void updatehistory(BoardState** pphead);

// 悔棋操作
void undoMove(BoardState** pphead); 