#include "chinesechess.h"

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