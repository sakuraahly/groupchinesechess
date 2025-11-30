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
#define BING 1        // 兵/卒
#define JU 2        // 车
#define PAO 3         // 炮
#define MA 4          // 马
#define XIANG 5       // 象/相
#define SHI 6        // 士/仕
#define JIANG 7        // 将/帅

// 使用两位数定义棋子：十位代表颜色，个位代表种类
#define RED_BING     (RED * 10 + BING)     // 红兵 = 11
#define BLACK_BING   (BLACK * 10 + BING)   // 黑卒 = 21
#define RED_JU     (RED * 10 + JU)     // 红车 = 12
#define BLACK_JU   (BLACK * 10 + JU)   // 黑车 = 22
#define RED_PAO      (RED * 10 + PAO)      // 红炮 = 13
#define BLACK_PAO    (BLACK * 10 + PAO)    // 黑炮 = 23
#define RED_MA       (RED * 10 + MA)       // 红马 = 14
#define BLACK_MA     (BLACK * 10 + MA)     // 黑马 = 24
#define RED_XIANG    (RED * 10 + XIANG)    // 红相 = 15
#define BLACK_XIANG  (BLACK * 10 + XIANG)  // 黑象 = 25
#define RED_SHI    (RED * 10 + SHI)     // 红仕 = 16
#define BLACK_SHI   (BLACK * 10 + SHI)   // 黑士 = 26
#define RED_JIANG     (RED * 10 + JIANG)     // 红帅 = 17
#define BLACK_JIANG   (BLACK * 10 + JIANG)   // 黑将 = 27

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