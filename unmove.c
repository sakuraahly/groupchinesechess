#include "chinesechess.h"

//记录棋盘,把1拷到2上
void Copyboard(int BOARD2[BOARD_ROWS][BOARD_COLS],int BOARD1[BOARD_ROWS][BOARD_COLS])
{
    for(int i=0;i<=BOARD_ROWS;i++)
    {
        for(int j=0;j<=BOARD_COLS;j++)
        {
             BOARD2[i][j]=BOARD1[i][j];
        }
    }
    return 0;
}

// 初始化棋盘，设置棋子的初始位置
void initplayboard(int BOARD[BOARD_ROWS][BOARD_COLS])    //没写完
 {     
    // 初始化红兵位置（位于棋盘第6行）
    BOARD[6][0] = RED_SOLDIER;  // 红兵在第一列
    BOARD[6][2] = RED_SOLDIER;  // 红兵在第三列
    BOARD[6][4] = RED_SOLDIER;  // 红兵在第五列
    BOARD[6][6] = RED_SOLDIER;  // 红兵在第七列
    BOARD[6][8] = RED_SOLDIER;  // 红兵在第九列
    
    // 初始化黑卒位置（位于棋盘第3行）
    BOARD[3][0] = BLACK_SOLDIER;  // 黑卒在第一列
    BOARD[3][2] = BLACK_SOLDIER;  // 黑卒在第三列
    BOARD[3][4] = BLACK_SOLDIER;  // 黑卒在第五列
    BOARD[3][6] = BLACK_SOLDIER;  // 黑卒在第七列
    BOARD[3][8] = BLACK_SOLDIER;  // 黑卒在第九列
    
    // 初始化车的位置
    BOARD[9][0] = RED_CHARIOT;   // 红车在左下角
    BOARD[9][8] = RED_CHARIOT;   // 红车在右下角
    BOARD[0][0] = BLACK_CHARIOT; // 黑车在左上角
    BOARD[0][8] = BLACK_CHARIOT; // 黑车在右上角
    
    // 初始化炮的位置
    BOARD[7][1] = RED_CANNON;    // 红炮在第七行第二列
    BOARD[7][7] = RED_CANNON;    // 红炮在第七行第八列
    BOARD[2][1] = BLACK_CANNON;  // 黑炮在第二行第二列
    BOARD[2][7] = BLACK_CANNON;  // 黑炮在第二行第八列
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
    copyboard(node->playboard, BOARD[BOARD_ROWS][BOARD_COLS]);
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

//走棋后链表更新
void updatehistory(BoardState** pphead) //此时传入 &pcur
{
    assert(pphead!=NULL&&*pphead!=NULL);

    BoardState* node=buynode(board);
    node->prev=*pphead;
    *pphead=node;
}

// 悔棋操作
void undoMove(BoardState** pphead) //传入&pcur
{
    assert(pphead!=NULL&&*pphead!=NULL);

    BoardState* tmp=(*pphead)->prev;
    free(*pphead);
    *pphead=tmp;

    Copyboard(board, (*pphead)->playboard);
    printf("悔棋成功");
}