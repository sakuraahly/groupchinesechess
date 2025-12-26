#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include<assert.h>
#include<time.h>

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

extern int board[BOARD_ROWS][BOARD_COLS];

// 河流边界定义
#define RIVER_RED 5      // 红方过河边界
#define RIVER_BLACK 4    // 黑方过河边界

int get_piece_color(int piece);

int get_piece_type(int piece);

bool is_red_piece(int piece);

bool is_black_piece(int piece);

void init_board();

void display_board();

bool is_valid_position(int x, int y);

bool is_same_side(int piece1, int piece2);

int count_pieces_between(int x1, int y1, int x2, int y2);

bool is_bing_move_valid(int from_x, int from_y, int to_x, int to_y, int piece);

bool is_ju_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) ;

bool is_pao_move_valid(int from_x, int from_y, int to_x, int to_y, int piece); 

bool is_ma_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) ;

bool is_xiang_move_valid(int from_x, int from_y, int to_x, int to_y, int piece);

bool is_shi_move_valid(int from_x, int from_y, int to_x, int to_y, int piece);

bool is_jiang_move_valid(int from_x, int from_y, int to_x, int to_y, int piece) ;

bool is_move_valid(int from_x, int from_y, int to_x, int to_y);

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

// 思路二=======================================================================================

// 移动记录结构体
typedef struct MoveRecord {
    int from_x, from_y;     // 移动前的位置
    int to_x, to_y;         // 移动后的位置
    int chipiece;     // 被吃掉的棋子（如果有）
    int napiece;        // 移动的棋子
    struct MoveRecord* prev; // 上一步
    struct MoveRecord* next; // 下一步（用于重做功能，可选）
} MoveRecord;

// 移动历史管理
typedef struct {
    MoveRecord* current;    // 当前移动记录
    MoveRecord* tail;       // 历史记录尾部（最新记录）
    int move_count;         // 总移动步数
} MoveHistory;

// 函数声明
MoveHistory* create_move_history();
void record_move(MoveHistory* history, int from_x, int from_y, int to_x, int to_y, int captured_piece, int moved_piece);
bool undo_move(MoveHistory* history);
bool redo_move(MoveHistory* history);
void clear_move_history(MoveHistory* history);
bool make_move_with_record(MoveHistory* history, int from_x, int from_y, int to_x, int to_y);

