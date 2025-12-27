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

// 这个判断条件是为了保证只播放一次音效的.
extern bool is_music_playing;
bool play_winsound = true;
bool *play_winsound_ptr = &play_winsound;
// 致命提醒!!!!!不要删除错误捕获代码,没人知道为什么会错误.
int main(int argc, char *argv[])
{ // 塞一个void试试?

    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        // printf("SDL初始化失败: %s\n", SDL_GetError());
        return -1;
    }

    // 初始化SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        // printf("SDL_image初始化失败: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    // 初始化SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        // printf("SDL_mixer初始化失败: %s\n", Mix_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // 创建窗口和渲染器(可以改变显示框顶部的标题)
    SDL_Window *window = SDL_CreateWindow("中国象棋", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    { // 强调一下,这个创建失败是出现一个空指针,NULL在bool上是false的等价.
        // printf("创建窗口失败: %s\n", SDL_GetError());
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        // printf("创建渲染器失败: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    is_music_playing = false;
    // 加载背景音乐(以及状态判定)
    // Mix_Music* bgm = Mix_LoadMUS("res/music/bgm.mp3");
    init_music();

    if (bgm)
    {
        Mix_PlayMusic(bgm, -1);
        is_music_playing = true;
    }

    // 加载别的音频以及对应的状态
    //  Mix_Music* choseChess = Mix_LoadMUS("res/music/chose.mp3");
    //  static bool shouldPlayChoseChess = false;//这个的音频控制在chess_move.c中,is_piece_selected这个函数里面

    // 加载资源(这些是菜单和背景)
    SDL_Texture *background = loadTexture(renderer, "res/images/background.png");
    SDL_Texture *chess_board = loadTexture(renderer, "res/images/chess_board.png");
    SDL_Texture *start_button = loadTexture(renderer, "res/images/start_button.png");
    SDL_Texture *special_mode = loadTexture(renderer, "res/images/special_mode.png");

    // 加载棋子纹理（使用新编码索引）
    SDL_Texture *pieces[28] = {NULL};
    for (int i = 11; i <= 27; i++)
    {
        if (piece_names[i] != NULL)
        { // 只加载有对应图片的编码
            char path[256];
            snprintf(path, sizeof(path), "./res/images/%s", piece_names[i]);
            pieces[i] = loadTexture(renderer, path);
        }
    }

    // ====== 新增：加载侧边按钮图标 ====== 这些是游戏界面的侧边按钮
    SDL_Texture *return_button = loadTexture(renderer, "res/images/return_to_menu.png");
    SDL_Texture *revoke_button = loadTexture(renderer, "res/images/revoke_chess.png");
    // 新增保存按钮
    SDL_Texture *save_button = loadTexture(renderer, "res/images/save_button.png");
    // 新增"撤销悔棋"按钮
    SDL_Texture *redo_button = loadTexture(renderer, "res/images/redo_button.png");

    // 新增：加载"继续游戏"按钮图片（用于菜单界面）
    SDL_Texture *continue_button = loadTexture(renderer, "res/images/continue_button.png");

    // 新加载胜利图片 -hu 12.26
    SDL_Texture *red_victory_image = loadTexture(renderer, "res/images/red_win.png");
    SDL_Texture *black_victory_image = loadTexture(renderer, "res/images/black_win.png");

    // 创建开始按钮
    SDL_Rect startButtonRect = {
        (SCREEN_WIDTH - 200) / 2,
        (SCREEN_HEIGHT - 80) / 2 + 100,
        200, 80};

    // "继续游戏"按钮的位置定义
    SDL_Rect continueButtonRect = {
        (SCREEN_WIDTH - 200) / 2,
        (SCREEN_HEIGHT - 80) / 2 + 200,
        200, 80};

    // 创建侧边按钮位置
    SDL_Rect returnButtonRect = {
        30,  // 距离左侧30像素
        100, // 距离顶部100像素
        100, // 宽度
        100  // 高度
    };

    SDL_Rect revokeButtonRect = {
        30,  // 和返回按钮在同一列
        250, // 在返回按钮下方150像素（100+100+50间距）
        159, // 宽度
        86   // 高度
    };

    // 新增保存按钮位置
    SDL_Rect saveButtonRect = {
        30,  // 和悔棋按钮在同一列
        400, // 在悔棋按钮下方
        100, // 宽度
        50   // 高度
    };

    // ====== 新增："撤销悔棋"按钮位置 ======
    SDL_Rect redoButtonRect = {
        30,  // 和保存按钮在同一列
        480, // 在保存按钮下方（400+50+30间距）
        100, // 宽度
        50   // 高度
    };

    // 增加 胜利与失败的图片的位置. -hu 12.26
    SDL_Rect redVictoryRect = {
        30,  // 距离左侧30像素
        560, // 距离顶部560像素
        200, // 宽度
        200  // 高度
    };

    SDL_Rect specialModeRect = {
        30,  // 距离左侧30像素
        560, // 距离顶部560像素
        200, // 宽度
        200  // 高度
    };

    // 游戏状态
    GameState currentState = MENU_STATE;

    // // 显示坐标系信息
    // printf("=== 坐标系设置 ===\n");
    // printf("棋盘显示位置: (%d, %d)\n", BOARD_VISUAL_X, BOARD_VISUAL_Y);
    // printf("棋盘格点原点: (%d, %d)\n", GRID_ORIGIN_X, GRID_ORIGIN_Y);
    // printf("网格间距: %dx%d\n", GRID_WIDTH, GRID_HEIGHT);
    // printf("棋子尺寸: %d\n", PIECE_SIZE);
    // printf("\n关键棋子理论位置:\n");
    // printf("黑车(0,0): 格点(%d,%d)\n",
    //        GRID_ORIGIN_X + 0 * GRID_WIDTH, GRID_ORIGIN_Y + 0 * GRID_HEIGHT);
    // printf("黑将(0,4): 格点(%d,%d)\n",
    //        GRID_ORIGIN_X + 4 * GRID_WIDTH, GRID_ORIGIN_Y + 0 * GRID_HEIGHT);
    // printf("红帅(9,4): 格点(%d,%d)\n",
    //        GRID_ORIGIN_X + 4 * GRID_WIDTH, GRID_ORIGIN_Y + 9 * GRID_HEIGHT);

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
                    // printf("新游戏开始！红方先走。\n");
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

                // 菜单状态下点击特色模式按钮 -hu 12.27
                if (currentState == MENU_STATE && pointInRect(mouseX, mouseY, specialModeRect))
                {
                    currentState = SPECIAL_MODE_STATE;
                    restoreBoardToStandardState();
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
            else
            {
                SDL_SetRenderDrawColor(renderer, 50, 50, 100, 255);
                SDL_RenderClear(renderer);
            }
            // 开始按钮,位于大概正中间
            if (start_button)
            {
                SDL_RenderCopy(renderer, start_button, NULL, &startButtonRect);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 150, 200, 255);
                SDL_RenderFillRect(renderer, &startButtonRect);
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_RenderDrawRect(renderer, &startButtonRect);
            }

            // 特色模式按钮 -hu 12.27
            if (special_mode)
            {
                SDL_RenderCopy(renderer, special_mode, NULL, &specialModeRect);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 150, 200, 255);
                SDL_RenderFillRect(renderer, &specialModeRect);
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_RenderDrawRect(renderer, &specialModeRect);
            }
        }

        // 渲染普通模式游戏界面 -hu 12.27
        else if (currentState == GAME_STATE)
        {
            if (background)
            {
                // 新增：渲染"继续游戏"按钮（如果有存档）
                if (has_save())
                {
                    if (continue_button)
                    {
                        SDL_RenderCopy(renderer, continue_button, NULL, &continueButtonRect);
                    }
                    else
                    {
                        // 如果图标加载失败，绘制默认按钮
                        SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255); // 蓝色
                        SDL_RenderFillRect(renderer, &continueButtonRect);
                        // 绘制边框
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_RenderDrawRect(renderer, &continueButtonRect);
                    }
                }
            }
            else
            {
                // 渲染游戏界面
                if (background)
                {
                    SDL_RenderCopy(renderer, background, NULL, NULL);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 50, 50, 100, 255);
                    SDL_RenderClear(renderer);
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
                else
                {
                    // 如果没有棋盘图片，绘制简单棋盘
                    SDL_SetRenderDrawColor(renderer, 210, 180, 140, 255);
                    SDL_Rect boardRect = {
                        BOARD_VISUAL_X,
                        BOARD_VISUAL_Y,
                        BOARD_DISPLAY_WIDTH,
                        BOARD_DISPLAY_HEIGHT};
                    SDL_RenderFillRect(renderer, &boardRect);
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
                            else
                            {
                                // 棋子纹理加载失败，绘制占位矩形
                                int screen_x = GRID_ORIGIN_X + y * GRID_WIDTH - PIECE_SIZE / 2;
                                int screen_y = GRID_ORIGIN_Y + x * GRID_HEIGHT - PIECE_SIZE / 2;
                                SDL_Rect dest = {screen_x, screen_y, PIECE_SIZE, PIECE_SIZE};
                                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                                SDL_RenderFillRect(renderer, &dest);
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
                else
                {
                    SDL_SetRenderDrawColor(renderer, 50, 50, 100, 255);
                    SDL_RenderClear(renderer);
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
                else
                {
                    // 如果没有棋盘图片，绘制简单棋盘
                    SDL_SetRenderDrawColor(renderer, 210, 180, 140, 255);
                    SDL_Rect boardRect = {
                        BOARD_VISUAL_X,
                        BOARD_VISUAL_Y,
                        BOARD_DISPLAY_WIDTH,
                        BOARD_DISPLAY_HEIGHT};
                    SDL_RenderFillRect(renderer, &boardRect);
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
                            else
                            {
                                // 棋子纹理加载失败，绘制占位矩形
                                int screen_x = GRID_ORIGIN_X + y * GRID_WIDTH - PIECE_SIZE / 2;
                                int screen_y = GRID_ORIGIN_Y + x * GRID_HEIGHT - PIECE_SIZE / 2;
                                SDL_Rect dest = {screen_x, screen_y, PIECE_SIZE, PIECE_SIZE};
                                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                                SDL_RenderFillRect(renderer, &dest);
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
                else
                {
                    // 如果图标加载失败，绘制默认按钮
                    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
                    SDL_RenderFillRect(renderer, &returnButtonRect);
                    // 绘制边框
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderDrawRect(renderer, &returnButtonRect);
                }

                // 渲染"悔棋"按钮
                if (revoke_button && currentState)
                {
                    SDL_RenderCopy(renderer, revoke_button, NULL, &revokeButtonRect);
                }
                else
                {
                    // 如果图标加载失败，绘制默认按钮
                    SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255);
                    SDL_RenderFillRect(renderer, &revokeButtonRect);
                    // 绘制边框
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderDrawRect(renderer, &revokeButtonRect);
                }

                // 渲染"保存棋局"按钮
                if (save_button && currentState)
                {
                    SDL_RenderCopy(renderer, save_button, NULL, &saveButtonRect);
                }
                else
                {
                    // 如果图标加载失败，绘制默认按钮
                    SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
                    SDL_RenderFillRect(renderer, &saveButtonRect);
                    // 绘制边框
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderDrawRect(renderer, &saveButtonRect);
                }

                // ====== 新增：渲染"撤销悔棋"按钮 ======
                if (redo_button && currentState)
                {
                    SDL_RenderCopy(renderer, redo_button, NULL, &redoButtonRect);
                }
                else
                {
                    // 如果图标加载失败，绘制默认按钮
                    SDL_SetRenderDrawColor(renderer, 200, 150, 50, 255); // 橙色
                    SDL_RenderFillRect(renderer, &redoButtonRect);
                    // 绘制边框
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderDrawRect(renderer, &redoButtonRect);
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
