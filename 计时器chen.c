#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h> 
typedef struct {
    int minutes;    
    int seconds;   
    int total_time; 
    int is_running; 
} Player;

Player player1, player2;
Player *current_player;


void init_players(int minutes) {
    player1.minutes = minutes;
    player1.seconds = 0;
    player1.total_time = minutes * 60;
    player1.is_running = 0;
    
    player2.minutes = minutes;
    player2.seconds = 0;
    player2.total_time = minutes * 60;
    player2.is_running = 0;
}


void display_time() {
    system("cls"); 
    printf("计时器\n");
    
    
    if(current_player == &player1) {
        printf(">>> 红方: %02d:%02d <<<\n", 
               player1.minutes, player1.seconds);
    } else {
        printf("    红方: %02d:%02d    \n", 
               player1.minutes, player1.seconds);
    }
    
    
    if(current_player == &player2) {
        printf(">>> 黑方: %02d:%02d <<<\n", 
               player2.minutes, player2.seconds);
    } else {
        printf("    黑方: %02d:%02d    \n", 
               player2.minutes, player2.seconds);
    }
    
    printf("\n按空格键切换玩家，ESC键退出\n");
}


void update_time() {
    if(current_player->is_running) {
        if(current_player->total_time > 0) {
            current_player->total_time--;
            current_player->minutes = current_player->total_time / 60;
            current_player->seconds = current_player->total_time % 60;
        }
    }
}


void switch_player() {
    if(current_player == &player1) {
        player1.is_running = 0;
        player2.is_running = 1;
        current_player = &player2;
    } else {
        player2.is_running = 0;
        player1.is_running = 1;
        current_player = &player1;
    }
}


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

int main() {
    int initial_time;
    
    printf("设置时间（分钟）: ");
    scanf("%d", &initial_time);
    
    
    init_players(initial_time);
    
    
    current_player = &player1;
    player1.is_running = 1;
    
    printf("红方先行。\n");
    printf("按任意键继续...");
    getch();
    
    
    while(1) {
        display_time();
        
        
        if(kbhit()) {
            char key = getch();
            
            if(key == ' ') { // 按空格切换玩家
                switch_player();
            } else if(key == 27) { // ESC键退出
                printf("\n游戏结束！\n");
                break;
            }
        }
        
       
        update_time();
        
        
        if(check_timeout()) {
            break;
        }
        
       
        Sleep(1000);
    }
    
    return 0;
}
