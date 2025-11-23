#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h> 

// 定义玩家结构体，存储每个棋手的时间
typedef struct {
    int minutes;    // 分钟数
    int seconds;    // 秒数
    int total_time; // 总时间（以秒为单位）
    int is_running; // 标记该玩家的计时器是否正在运行
} Player;

// 全局变量声明
Player player1, player2;  // 两个玩家
Player *current_player;   // 指向当前正在计时的玩家

/**
 * 初始化玩家时间
 * @param minutes 初始分钟数
 */
void init_players(int minutes) {
    // 初始化红方玩家
    player1.minutes = minutes;
    player1.seconds = 0;
    player1.total_time = minutes * 60;  // 将分钟转换为秒
    player1.is_running = 0;             // 初始状态为停止计时
    
    // 初始化黑方玩家
    player2.minutes = minutes;
    player2.seconds = 0;
    player2.total_time = minutes * 60;  // 将分钟转换为秒
    player2.is_running = 0;             // 初始状态为停止计时
}

/**
 * 显示当前时间状态
 */
void display_time() {
    system("cls");  // 清空控制台屏幕
    printf("计时器\n");
    
    // 显示红方时间，如果当前是红方回合则用箭头标记
    if(current_player == &player1) {
        printf(">>> 红方: %02d:%02d <<<\n", 
               player1.minutes, player1.seconds);
    } else {
        printf("    红方: %02d:%02d    \n", 
               player1.minutes, player1.seconds);
    }
    
    // 显示黑方时间，如果当前是黑方回合则用箭头标记
    if(current_player == &player2) {
        printf(">>> 黑方: %02d:%02d <<<\n", 
               player2.minutes, player2.seconds);
    } else {
        printf("    黑方: %02d:%02d    \n", 
               player2.minutes, player2.seconds);
    }
    
    printf("\n操作说明：按空格键切换玩家，ESC键退出游戏\n");
}

/**
 * 更新当前玩家的时间
 */
void update_time() {
    // 只有当前玩家的计时器在运行时才更新时间
    if(current_player->is_running) {
        if(current_player->total_time > 0) {
            current_player->total_time--;  // 总时间减少1秒
            // 重新计算分钟和秒数
            current_player->minutes = current_player->total_time / 60;
            current_player->seconds = current_player->total_time % 60;
        }
    }
}

/**
 * 切换当前玩家
 */
void switch_player() {
    if(current_player == &player1) {
        // 从红方切换到黑方
        player1.is_running = 0;  // 停止红方计时
        player2.is_running = 1;  // 启动黑方计时
        current_player = &player2;  // 当前玩家指向黑方
    } else {
        // 从黑方切换到红方
        player2.is_running = 0;  // 停止黑方计时
        player1.is_running = 1;  // 启动红方计时
        current_player = &player1;  // 当前玩家指向红方
    }
}

/**
 * 检查是否有玩家超时
 * @return 1表示有玩家超时，0表示没有
 */
int check_timeout() {
    if(player1.total_time <= 0) {
        printf("\n红方超时！黑方获胜！\n");
        return 1;
    }
    if(player2.total_time <= 0) {
        printf("\n黑方超时！红方获胜！\n");
        return 1;
    }
    return 0;
}

/**
 * 主函数
 */
int main() {
    int initial_time;  // 初始时间设置
    
    // 获取用户输入的初始时间
    printf("设置每位玩家的初始时间（分钟）: ");
    scanf("%d", &initial_time);
    
    // 初始化玩家
    init_players(initial_time);
    
    // 设置红方为当前玩家并启动计时
    current_player = &player1;
    player1.is_running = 1;
    
    // 显示开始信息
    printf("红方先行。\n");
    printf("按任意键继续...");
    getch();  // 等待用户按键
    
    // 主游戏循环
    while(1) {
        display_time();  // 显示当前时间状态
        
        // 检测键盘输入
        if(kbhit()) {
            char key = getch();
            
            if(key == ' ') {     // 空格键：切换玩家
                switch_player();
            } else if(key == 27) { // ESC键：退出游戏
                printf("\n游戏结束！\n");
                break;
            }
        }
        
        // 更新时间
        update_time();
        
        // 检查是否超时
        if(check_timeout()) {
            break;  // 如果有玩家超时，退出循环
        }
        
        // 暂停1秒，控制计时精度
        Sleep(1000);
    }
    
    return 0;
}
