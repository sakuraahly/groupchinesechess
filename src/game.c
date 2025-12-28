#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <SDL2/SDL.h>
// #include <SDL2/SDL_main.h> //总之在目前的开发中不需要就是了
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

// ==================== 包含数据库头文件 ====================
#include "chess_move.h"
#include "chess_database.h"
#include "displayinterface.h"

// 游戏状态,判断是否结束了.
typedef enum
{
    MENU_STATE,
    GAME_STATE,
    SPECIAL_MODE_STATE
} GameState;

// 这个判断条件是为了保证只播放一次音效的. -hu 12.28
extern bool is_music_playing;
bool play_winsound = true;
bool *play_winsound_ptr = &play_winsound;

bool play_surrendersound = false;
bool *play_surrendersound_ptr = &play_surrendersound;
bool is_surrender = false;
bool *is_surrender_ptr = &is_surrender;
// 致命提醒!!!!!不要删除错误捕获代码,没人知道为什么会错误.
int main(int argc, char *argv[])
{ // 塞一个void试试?

    // 加载画面资源和SDL库的代码 -hu 12.28
    init_resources();

    is_music_playing = false;
    // 加载背景音乐(以及状态判定)
    // Mix_Music* bgm = Mix_LoadMUS("res/music/bgm.mp3");
    init_music();

        // 游戏状态
    GameState currentState = MENU_STATE;

    // 主循环,大部分改变要在这里进行
    // 上面的是事件处理循环,下面的是渲染循环 -hu 11.24
    bool running = true;
    while (running)
    {
        SDL_Event event;

        // 下面是事件处理循环.
        while (SDL_PollEvent(&event))
        {
            // if (is_jiang_live == false || is_shuai_live == false){
            //     // 死棋的情况下,不再响应任何走子事件.
            //     //goto victoryscreen;
            // }

            // 处理背景音乐的问题,这个设定大概是无限播放 12.26
            if (is_music_playing == false)
            {
                is_music_playing = true;
                Mix_PlayMusic(bgm, 0);
            }

            if (event.type == SDL_QUIT)
            {
                running = false;
            }

            // 鼠标点击事件
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                // 菜单状态下点击开始按钮
                if (currentState == MENU_STATE && pointInRect(mouseX, mouseY, startButtonRect))
                {
                    currentState = GAME_STATE;
                    // 将调用的棋盘恢复成标准状态 -hu 12.27
                    restoreBoardToStandardState();
                    // 初始化棋局记录
                    init_game_record(&current_game, "GAME001", "红方玩家", "黑方玩家");
                    move_start_time = time(NULL);
                    // 重置游戏状态（确保新游戏从干净状态开始）//注意,将帅活着,红先走 -hu 12.27
                    is_shuai_live = true;
                    is_jiang_live = true;
                    redFlyToWin = false;
                    blackFlyToWin = false;
                    is_red_turn = true;
                    *is_surrender_ptr = false;
                    *play_surrendersound_ptr = false;
                }

                // 菜单状态下点击特色模式按钮 -hu 12.27
                if (currentState == MENU_STATE && pointInRect(mouseX, mouseY, specialModeRect))
                {
                    currentState = SPECIAL_MODE_STATE;
                    restoreBoardToStandardState();
                    // 重置游戏状态（确保新游戏从干净状态开始）//注意,将帅活着,红先走 -hu 12.27
                    is_shuai_live = true;
                    is_jiang_live = true;
                    redFlyToWin = false;
                    blackFlyToWin = false;
                    is_red_turn = true;
                    *is_surrender_ptr = false;
                    *play_surrendersound_ptr = false;
                    // 特色模式是除了将和帅在得那两行,其余的都是兵和卒
                    for (int x = 1; x < 5; x++)
                    {
                        for (int y = 0; y < 9; y++)
                        {
                            board[x][y] = BLACK_ZU;
                        }
                    }
                    for (int x = 5; x < 9; x++)
                    {
                        for (int y = 0; y < 9; y++)
                        {
                            board[x][y] = RED_BING;
                        }
                    }

                    // 初始化棋局记录
                    init_game_record(&current_game, "GAME001", "红方玩家", "黑方玩家");
                    move_start_time = time(NULL);
                    // printf("新游戏开始！红方先走。\n");
                    // 重置游戏状态（确保新游戏从干净状态开始）
                    is_shuai_live = true;
                    is_jiang_live = true;
                    redFlyToWin = false;
                    blackFlyToWin = false;
                    printf("新游戏开始！红方先走。\n");
                }

                // 新增：菜单状态下点击"继续游戏"按钮
                if (currentState == MENU_STATE && pointInRect(mouseX, mouseY, continueButtonRect))
                {
                    if (has_save())
                    {
                        if (load_game())
                        {
                            currentState = GAME_STATE;
                            printf("继续上次游戏\n");
                            // 重新开始计时
                            move_start_time = time(NULL);
                        }
                    }
                    else
                    {
                        printf("没有找到存档文件\n");
                    }
                }

                // 游戏状态下点击
                if (currentState == GAME_STATE)
                {
                    // 检查"回到菜单"按钮
                    if (pointInRect(mouseX, mouseY, returnButtonRect))
                    {
                        printf("回到菜单\n");
                        currentState = MENU_STATE;
                        is_piece_selected = false;
                    }

                    // 检查"悔棋"按钮
                    if (pointInRect(mouseX, mouseY, revokeButtonRect))
                    {
                        printf("悔棋\n");
                        revokeLastMove();
                    }

                    // 检查"保存棋局"按钮
                    if (pointInRect(mouseX, mouseY, saveButtonRect))
                    {
                        printf("保存棋局\n");
                        save_game_to_file(&current_game, "chess_game_record.txt");
                    }

                    // ====== 新增：检查"撤销悔棋"按钮 ======
                    if (pointInRect(mouseX, mouseY, redoButtonRect))
                    {
                        printf("撤销悔棋\n");
                        // TODO: 这里将来实现撤销悔棋功能
                        // redoLastMove();
                    }
                    // 检查投降按钮 -hu 12.28
                    if (pointInRect(mouseX, mouseY, surrenderButtonRect))
                    {
                        *play_surrendersound_ptr = true;
                        *is_surrender_ptr = true;
                    }

                    // 检查是否点击了棋盘上的棋子
                    int board_x, board_y;
                    if (screenToBoard(mouseX, mouseY, &board_x, &board_y) && is_shuai_live == true && is_jiang_live == true)
                    {
                        handleBoardClick(board_x, board_y);
                    }
                }

                // 特色模式下点击
                if (currentState == SPECIAL_MODE_STATE)
                {
                    // 检查"回到菜单"按钮
                    if (pointInRect(mouseX, mouseY, returnButtonRect))
                    {
                        // printf("回到菜单\n");
                        currentState = MENU_STATE;
                        is_piece_selected = false;
                    }

                    // 检查"悔棋"按钮
                    if (pointInRect(mouseX, mouseY, revokeButtonRect))
                    {
                        // printf("悔棋\n");
                        revokeLastMove();
                    }

                    // 检查"保存棋局"按钮
                    if (pointInRect(mouseX, mouseY, saveButtonRect))
                    {
                        // printf("保存棋局\n");
                        save_game_to_file(&current_game, "chess_game_record.txt");
                    }

                    // ====== 新增：检查"撤销悔棋"按钮 ======
                    if (pointInRect(mouseX, mouseY, redoButtonRect))
                    {
                        // printf("撤销悔棋\n");
                        //  TODO: 这里将来实现撤销悔棋功能
                        //  redoLastMove();
                    }

                    // 检查投降按钮 -hu 12.28
                    if (pointInRect(mouseX, mouseY, surrenderButtonRect))
                    {
                        *play_surrendersound_ptr = true;
                        *is_surrender_ptr = true;
                    }

                    // 检查是否点击了棋盘上的棋子
                    int board_x, board_y;
                    if (screenToBoard(mouseX, mouseY, &board_x, &board_y) && is_shuai_live == true && is_jiang_live == true)
                    {
                        handleBoardClick(board_x, board_y);
                    }
                }
            }
        } // 这就是走子判定的结尾部分了

        // 下面就是渲染部分!
        //  清屏
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 渲染界面
        if (currentState == MENU_STATE)
        {
            // 渲染菜单界面
            if (background)
            {
                SDL_RenderCopy(renderer, background, NULL, NULL);
            }

            // 开始按钮,位于大概正中间
            if (start_button)
            {
                SDL_RenderCopy(renderer, start_button, NULL, &startButtonRect);
            }

            // 渲染继续游戏按钮 -hu 12.27
            if (continue_image)
            {
                SDL_RenderCopy(renderer, continue_image, NULL, &continueButtonRect);
            }

            // 特色模式按钮 -hu 12.27
            if (special_mode)
            {
                SDL_RenderCopy(renderer, special_mode, NULL, &specialModeRect);
            }
        }

        // 渲染普通模式游戏界面 -hu 12.27
        else if (currentState == GAME_STATE)
        {

            // 渲染游戏界面
            if (background)
            {
                SDL_RenderCopy(renderer, background, NULL, NULL);
            }

            // 渲染棋盘（使用棋盘显示位置）
            if (chess_board)
            {
                SDL_Rect boardRect = {
                    BOARD_VISUAL_X,
                    BOARD_VISUAL_Y,
                    BOARD_DISPLAY_WIDTH,
                    BOARD_DISPLAY_HEIGHT};
                SDL_RenderCopy(renderer, chess_board, NULL, &boardRect);
            }

            // 绘制棋子 - 使用独立的格点坐标系
            int pieceCount = 0;
            for (int x = 0; x < 10; x++)
            { // 行 (x: 0-9)
                for (int y = 0; y < 9; y++)
                { // 列 (y: 0-8)
                    int piece = board[x][y];
                    if (piece != NONE)
                    {
                        pieceCount++;

                        if (piece >= 11 && piece <= 27 && pieces[piece])
                        {
                            // 使用格点坐标系计算棋子位置（与棋盘显示位置无关）
                            int screen_x = GRID_ORIGIN_X + y * GRID_WIDTH - PIECE_SIZE / 2;
                            int screen_y = GRID_ORIGIN_Y + x * GRID_HEIGHT - PIECE_SIZE / 2;

                            SDL_Rect dest = {screen_x, screen_y, PIECE_SIZE, PIECE_SIZE};
                            SDL_RenderCopy(renderer, pieces[piece], NULL, &dest);
                        }
                    }
                }
            }
        } // 这里是普通模式的结尾部分 -hu 12.22

        if (currentState == SPECIAL_MODE_STATE)
        {
            // 绘制背景图片
            if (background)
            {
                SDL_RenderCopy(renderer, background, NULL, NULL);
            }

            // 渲染棋盘（使用棋盘显示位置）
            if (chess_board)
            {
                SDL_Rect boardRect = {
                    BOARD_VISUAL_X,
                    BOARD_VISUAL_Y,
                    BOARD_DISPLAY_WIDTH,
                    BOARD_DISPLAY_HEIGHT};
                SDL_RenderCopy(renderer, chess_board, NULL, &boardRect);
            }

            // 绘制棋子 - 使用独立的格点坐标系  //特色模式下,除了将帅在的那一行,都是充满了兵和卒子
            int pieceCount = 0;
            // 棋盘的上半部分除了将在的那一行改成黑色的卒子

            for (int x = 0; x < 10; x++)
            { // 行 (x: 0-9)
                for (int y = 0; y < 9; y++)
                { // 列 (y: 0-8)
                    int piece = board[x][y];
                    if (piece != NONE)
                    {
                        pieceCount++;

                        if (piece >= 11 && piece <= 27 && pieces[piece])
                        {
                            // 使用格点坐标系计算棋子位置（与棋盘显示位置无关）
                            int screen_x = GRID_ORIGIN_X + y * GRID_WIDTH - PIECE_SIZE / 2;
                            int screen_y = GRID_ORIGIN_Y + x * GRID_HEIGHT - PIECE_SIZE / 2;

                            SDL_Rect dest = {screen_x, screen_y, PIECE_SIZE, PIECE_SIZE};
                            SDL_RenderCopy(renderer, pieces[piece], NULL, &dest);
                        }
                    }
                }
            }
        } // 这里是特色模式的结尾部分 -hu 12.27

        // 下面是游戏模式通用的绘制的图片的模式

        // 绘制选中指示器
        drawSelectedIndicator(renderer);

        // 这两个有空再开发吧 -hu 12.4
        //  绘制当前玩家指示器
        // drawCurrentPlayerIndicator(renderer);

        // 绘制游戏信息
        // drawGameInfo(renderer);

        // 侧边按钮渲染代码
        // 渲染"回到菜单"按钮
        if (currentState != MENU_STATE)
        {
            if (return_button && currentState)
            {
                SDL_RenderCopy(renderer, return_button, NULL, &returnButtonRect);
            }

            // 渲染"悔棋"按钮
            if (revoke_button && currentState)
            {
                SDL_RenderCopy(renderer, revoke_button, NULL, &revokeButtonRect);
            }

            // 渲染"保存棋局"按钮
            if (save_button && currentState)
            {
                SDL_RenderCopy(renderer, save_button, NULL, &saveButtonRect);
            }

            // ====== 新增：渲染"撤销悔棋"按钮 ======
            if (redo_button && currentState)
            {
                SDL_RenderCopy(renderer, redo_button, NULL, &redoButtonRect);
            }

            // 渲染投降的按钮 -hu 12.28
            if (surrender_button && currentState)
            {
                SDL_RenderCopy(renderer, surrender_button, NULL, &surrenderButtonRect);
            }

            // "以上是" 游戏中其余的按钮的渲染部分 -hu 12.28
            // 这里播放投降的音效:
            if (*is_surrender_ptr == true)
            {
                Mix_PlayChannel(-1, surrender, 0);
                *is_surrender_ptr = false;
            }

            // 这里展示胜败的画面:当然是红方胜利显示红方获胜,反之则是黑方  //-hu 12.26
            if (is_shuai_live == true && is_jiang_live == false && currentState)
            {
                SDL_RenderCopy(renderer, red_victory_image, NULL, &redVictoryRect); // 最后一个是距离边框的位置
            }
            // 下面这个只是为了保障音效只播放一次,不能插在上面 // -hu 12.27
            if (is_shuai_live == true && is_jiang_live == false && play_winsound == true && currentState)
            {
                Mix_PlayChannel(-1, win, 0);
                // *play_winsound_ptr = false;
                *play_winsound_ptr = false;
            }
            if (is_jiang_live == true && is_shuai_live == false && currentState)
            {
                SDL_RenderCopy(renderer, black_victory_image, NULL, &redVictoryRect);
            }
            if (is_jiang_live == true && is_shuai_live == false && play_winsound == true && currentState)
            {
                Mix_PlayChannel(-1, win, 0);
                // *play_winsound_ptr = false;
                *play_winsound_ptr = false;
            }
        }
        // 呈现画面
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // 清理资源
    // printf("清理资源...\n");
    if (bgm)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }

    // 这里的统一用SDL_DestroyTexture函数销毁纹理(实际上按照我们的理解翻译成图片资源更好. -hu 12.26)

    for (int i = 0; i < 28; i++)
    {
        if (pieces[i])
            SDL_DestroyTexture(pieces[i]);
    }
    if (background)
        SDL_DestroyTexture(background);
    if (chess_board)
        SDL_DestroyTexture(chess_board);
    if (start_button)
        SDL_DestroyTexture(start_button);

    // 清理侧边按钮纹理
    if (return_button)
        SDL_DestroyTexture(return_button);
    if (revoke_button)
        SDL_DestroyTexture(revoke_button);
    if (save_button)
        SDL_DestroyTexture(save_button);
    if (redo_button)
        SDL_DestroyTexture(redo_button);
    if (red_victory_image)
        SDL_DestroyTexture(red_victory_image);
    if (black_victory_image)
        SDL_DestroyTexture(black_victory_image);

    // 清理继续游戏按钮纹理
    if (continue_button)
        SDL_DestroyTexture(continue_button);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    // printf("游戏退出\n");
    return 0;
} // 游戏主函数结尾的花括号
