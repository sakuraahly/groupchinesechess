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


//函数声明
bool pointInRect(int x, int y, SDL_Rect rect);//检测点是否在矩形内

int getPieceColor(int piece);//获取棋子颜色

int getPieceType(int piece);//获取棋子类型

bool screenToBoard(int screen_x, int screen_y, int* board_x, int* board_y) ;//屏幕坐标转换为棋盘坐标

bool isSameColor(int piece1, int piece2);//判断两枚棋子是否同色

int countPiecesInLine(int x1, int y1, int x2, int y2) ;//计算两点之间有多少棋子

bool isValidMove(int piece_code, int from_x, int from_y, int to_x, int to_y);//判断是否为合法的移动

bool movePiece(int from_x, int from_y, int to_x, int to_y) ;//移动棋子

void revokeLastMove() ; // 撤销上一步的移动

void handleBoardClick(int board_x, int board_y) ;//处理棋盘点击事件



#endif  // CHESS_MOVE_H