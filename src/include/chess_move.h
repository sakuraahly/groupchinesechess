#ifndef CHESS_MOVE_H
#define CHESS_MOVE_H

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


//函数声明
bool pointInRect(int x, int y, SDL_Rect rect);//检测点是否在矩形内

int getPieceColor(int piece);//获取棋子颜色

int getPieceType(int piece);//获取棋子类型

bool screenToBoard(int screen_x, int screen_y, int* board_x, int* board_y) ;//屏幕坐标转换为棋盘坐标

bool isSameColor(int piece1, int piece2);//判断两枚棋子是否同色

int countPiecesInLine(int x1, int y1, int x2, int y2) ;//计算两点之间有多少棋子

bool isValidMove(int piece_code, int from_x, int from_y, int to_x, int to_y);//判断是否为合法的移动

MoveHistory* create_move_history();

bool make_move_with_record(MoveHistory* history, int from_x, int from_y, int to_x, int to_y);//移动棋子

bool undo_move(MoveHistory* history); // 悔棋

void clear_move_history(MoveHistory* history);//移动棋子后清空后续历史

bool redo_move(MoveHistory* history);//撤销悔棋

void handleBoardClick(int board_x, int board_y) ;//处理棋盘点击事件

void record_move(MoveHistory* history, int from_x, int from_y, int to_x, int to_y, int captured_piece, int moved_piece);






#endif  // CHESS_MOVE_H