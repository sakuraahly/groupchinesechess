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

//引入棋盘
extern board[BOARD_ROWS][BOARD_COLS];

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

// 检查移动是否合法（简化版象棋规则）
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

//移动棋子
bool make_move_with_record(MoveHistory* history, int from_x, int from_y, int to_x, int to_y) {
    if (!is_move_valid(from_x, from_y, to_x, to_y)) {
        return false;
    }
    
    int na_piece = board[from_x][from_y];
    int chi_piece = board[to_x][to_y];
    
    // 执行移动
    board[to_x][to_y] = na_piece;
    board[from_x][from_y] = NONE;
    
    // 记录移动
    record_move(history, from_x, from_y, to_x, to_y, chi_piece, na_piece);

    // 计算思考时间
    if (move_start_time > 0) {
        current_move.thinking_time = (int)(time(NULL) - move_start_time);
    }
    
    return true;
}


/*悔棋功能*/
bool undo_move(MoveHistory* history) 
{
    if (history == NULL || history->current == NULL) {
        printf("无法悔棋：已经到达初始状态\n");
        return false;
    }
    
    MoveRecord* current_move = history->current;
    
    // 执行反向移动：将棋子移回原位置
    board[current_move->from_x][current_move->from_y] = current_move->napiece;
    
    // 恢复被吃掉的棋子（如果有）
    board[current_move->to_x][current_move->to_y] = current_move->chipiece;
    
    // 移动到上一步记录
    history->current = current_move->prev;
    
    printf("悔棋成功\n");
    return true;
}

// 处理棋盘点击
void handleBoardClick(int board_x, int board_y) {
    int clicked_piece = board[board_x][board_y];
    
    if (!is_piece_selected) {
        // 第一次点击：选择棋子
        if (clicked_piece == NONE) {
            printf("点击了空位置\n");
            return;
        }
        
        // 检查是否轮到该棋子走
        int piece_color = getPieceColor(clicked_piece);
        if ((is_red_turn && piece_color != COLOR_RED) ||
            (!is_red_turn && piece_color != COLOR_BLACK)) {
            printf("现在轮到%s走棋！\n", is_red_turn ? "红方" : "黑方");
            return;
        }
        
        // 选择棋子
        is_piece_selected = true;
        Mix_Music* choseChess = Mix_LoadMUS("res/music/chose.mp3");
        Mix_PlayMusic(choseChess, 0);
        selected_x = board_x;
        selected_y = board_y;
        selected_piece = clicked_piece;
        
        printf("选择了%s (%d,%d)\n", 
               get_piece_name_cn(selected_piece), selected_x, selected_y);
    } else {
        // 第二次点击：移动棋子或取消选择
        if (board_x == selected_x && board_y == selected_y) {
            // 点击同一位置：取消选择
            is_piece_selected = false;
            printf("取消选择\n");
        } else {
            // 尝试移动棋子
            if (movePiece(selected_x, selected_y, board_x, board_y)) {
                is_piece_selected = false;
            } else {
                printf("移动失败，请重新选择目标位置\n");
            }
        }
    }
}

// 创建移动历史记录
MoveHistory* create_move_history() {
    MoveHistory* history = (MoveHistory*)malloc(sizeof(MoveHistory));
    assert(history!=NULL);
    if (history == NULL) {
        perror("malloc fail");
        return NULL;
    }
    history->current = NULL;
    history->tail = NULL;
    history->move_count = 0;
    return history;
}

// 记录移动
void record_move(MoveHistory* history, int from_x, int from_y, int to_x, int to_y, int chipiece, int napiece) {
    // 如果当前不是最新记录，清除后面的记录
    assert(history!=NULL);
    if (history->current != history->tail) {
        MoveRecord* temp = (history->current ? history->current->next : history->tail);//依据current是否为NULL,判断从哪开始清理
        while (temp != NULL) {
            MoveRecord* next = temp->next;
            free(temp);
            temp = next;
        }
    }
    
    MoveRecord* new_move = (MoveRecord*)malloc(sizeof(MoveRecord));
    assert(new_move!= NULL);
    if (new_move == NULL) {
        perror("malloc fail");
        return;
    }
    
    new_move->from_x = from_x;
    new_move->from_y = from_y;
    new_move->to_x = to_x;
    new_move->to_y = to_y;
    new_move->chipiece = chipiece;
    new_move->napiece = napiece;
    new_move->prev = history->tail;
    new_move->next = NULL;
    
    if (history->tail != NULL) {
        history->tail->next = new_move;
    }
    
    history->tail = new_move;
    history->current = new_move;
    
    if (history->move_count == 0) {
        // 第一个记录
        history->current = new_move;
    }
    
    history->move_count++;
}

// 撤悔操作
bool redo_move(MoveHistory* history) 
{
    if (history == NULL || history->current == NULL || history->current->next == NULL) {
        printf("无法撤悔：没有可撤悔的步骤\n");
        return false;
    }
    
    MoveRecord* next_move = history->current->next;
    
    // 重新执行移动
    board[next_move->to_x][next_move->to_y] = next_move->napiece;
    board[next_move->from_x][next_move->from_y] = NONE;
    
    // 移动到下一步记录
    history->current = next_move;
    
    printf("撤悔成功\n");
    return true;
}

// 清空移动历史（除了初始状态）
void clear_move_history(MoveHistory* history) {
    if (history == NULL) return;
    
    MoveRecord* current = history->tail;
    while (current != NULL) {
        MoveRecord* prev = current->prev;
        free(current);
        current = prev;
    }
    
    history->current = NULL;
    history->tail = NULL;
    history->move_count = 0;
}

