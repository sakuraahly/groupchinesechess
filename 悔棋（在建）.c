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



// 棋盘状态结构体
typedef struct {
    int board[BOARD_ROWS][BOARD_COLS];
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
    int capturedPiece;
} BoardState;

BoardState history[10];
int historyIndex = -1;

// 悔棋操作
void undoMove(int board[BOARD_ROWS][BOARD_COLS]) {
    if (historyIndex < 0) {
        printf("没有可以悔棋的步骤\n");
        return;
    }
    BoardState prevState = history[historyIndex];
    historyIndex--;
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            board[i][j] = prevState.board[i][j];
        }
    }
    board[prevState.fromRow][prevState.fromCol] = board[prevState.toRow][prevState.toCol];
    if (prevState.capturedPiece != EMPTY) {
        board[prevState.toRow][prevState.toCol] = prevState.capturedPiece;
    } else {
        board[prevState.toRow][prevState.toCol] = EMPTY;
    }
    printf("悔棋成功！\n");
}