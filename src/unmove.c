#include "chinesechess.h"
/*思路一
//记录棋盘,把1拷到2上
void Copyboard(int BOARD2[BOARD_ROWS][BOARD_COLS],int BOARD1[BOARD_ROWS][BOARD_COLS])
{
    for(int i=0;i<BOARD_ROWS;i++)
    {
        for(int j=0;j<BOARD_COLS;j++)
        {
             BOARD2[i][j]=BOARD1[i][j];
        }
    }
}

// 初始化棋盘，设置棋子的初始位置
void initplayboard(int BOARD[BOARD_ROWS][BOARD_COLS])    //没写完
 {     
    // 初始化红兵位置（位于棋盘第6行）
    BOARD[6][0] = RED_BING;  // 红兵在第一列
    BOARD[6][2] = RED_BING;  // 红兵在第三列
    BOARD[6][4] = RED_BING;  // 红兵在第五列
    BOARD[6][6] = RED_BING;  // 红兵在第七列
    BOARD[6][8] = RED_BING;  // 红兵在第九列
    
    // 初始化黑卒位置（位于棋盘第3行）
    BOARD[3][0] = BLACK_BING;  // 黑卒在第一列
    BOARD[3][2] = BLACK_BING;  // 黑卒在第三列
    BOARD[3][4] = BLACK_BING;  // 黑卒在第五列
    BOARD[3][6] = BLACK_BING;  // 黑卒在第七列
    BOARD[3][8] = BLACK_BING;  // 黑卒在第九列
    
    // 初始化车的位置
    BOARD[9][0] = RED_JU;   // 红车在左下角
    BOARD[9][8] = RED_JU;   // 红车在右下角
    BOARD[0][0] = BLACK_JU; // 黑车在左上角
    BOARD[0][8] = BLACK_JU; // 黑车在右上角
    
    // 初始化炮的位置
    BOARD[7][1] = RED_PAO;    // 红炮在第七行第二列
    BOARD[7][7] = RED_PAO;    // 红炮在第七行第八列
    BOARD[2][1] = BLACK_PAO;  // 黑炮在第二行第二列
    BOARD[2][7] = BLACK_PAO;  // 黑炮在第二行第八列
    
    // 初始化将/帅的位置
    BOARD[9][4] = RED_JIANG;   // 红帅在第九行第五列
    BOARD[0][4] = BLACK_JIANG; // 黑将在第一行第五列
    
    // 初始化士/仕的位置
    BOARD[9][3] = RED_SHI;     // 红仕在第九行第四列
    BOARD[9][5] = RED_SHI;     // 红仕在第九行第六列
    BOARD[0][3] = BLACK_SHI;   // 黑士在第一行第四列
    BOARD[0][5] = BLACK_SHI;   // 黑士在第一行第六列
    
    // 初始化象/相的位置
    BOARD[9][2] = RED_XIANG;   // 红相在第九行第三列
    BOARD[9][6] = RED_XIANG;   // 红相在第九行第七列
    BOARD[0][2] = BLACK_XIANG; // 黑象在第一行第三列
    BOARD[0][6] = BLACK_XIANG; // 黑象在第一行第七列
    
    // 初始化马的位置
    BOARD[9][1] = RED_MA;      // 红马在第九行第二列
    BOARD[9][7] = RED_MA;      // 红马在第九行第八列
    BOARD[0][1] = BLACK_MA;    // 黑马在第一行第二列
    BOARD[0][7] = BLACK_MA;    // 黑马在第一行第八列
}

//初始化结构体
void init_struct(BoardState* node)
{
    initplayboard(node->playboard);
    node->prev=NULL;
}

//创建节点
BoardState* buynode(int BOARD[BOARD_ROWS][BOARD_COLS])
{
    BoardState* node=(BoardState*)malloc(sizeof(BoardState));
    if(node==NULL)
    {
        perror("malloc fail");
        exit(1);
    }
    Copyboard(node->playboard, BOARD);
    node->prev=NULL;
    return node;
}

//定义用来记录整局的链表，创建对局
BoardState* Startgame()
{
    BoardState* Newgame=(BoardState*)malloc(sizeof(BoardState));
    init_struct(Newgame);
    return Newgame;
}

/*int main()
{
//当前棋盘状态为pcur
BoardState* pcur=Startgame();
}
......
//走棋成功后
updatehistory(&pcur);
*/

// 在 unmove.c 中实现移动记录的功能

#include "chinesechess.h"

extern board[BOARD_ROWS][BOARD_COLS];

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

// 悔棋操作
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
    board[next_move->from_x][next_move->from_y] = EMPTY;
    
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

// 修改走棋函数，在走棋时记录移动
bool make_move_with_record(MoveHistory* history, int from_x, int from_y, int to_x, int to_y) {
    if (!is_move_valid(from_x, from_y, to_x, to_y)) {
        return false;
    }
    
    int moved_piece = board[from_x][from_y];
    int captured_piece = board[to_x][to_y];
    
    // 执行移动
    board[to_x][to_y] = moved_piece;
    board[from_x][from_y] = EMPTY;
    
    // 记录移动
    record_move(history, from_x, from_y, to_x, to_y, captured_piece, moved_piece);
    
    return true;
}