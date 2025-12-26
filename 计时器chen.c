#include "timer.h"
#include <stdio.h>
#include <string.h>

// 保持原有函数名不变，但内部实现增强
void timer_init(Timer *timer, int time_limit) {
    timer->start_time = 0;
    timer->elapsed_time = 0;
    timer->time_limit = time_limit * 60; // 转换为秒
    timer->is_running = false;
    timer->is_paused = false;
    timer->pause_start = 0;
    timer->total_pause_time = 0;
}

int timer_get_elapsed_seconds(Timer *timer) {
    time_t elapsed = timer->elapsed_time;
    
    if (timer->is_running && !timer->is_paused) {
        time_t current_time = time(NULL);
        elapsed += (current_time - timer->start_time - timer->total_pause_time);
    } else if (timer->is_paused) {
        // 暂停状态下，已用时间不变
        elapsed += (timer->pause_start - timer->start_time - timer->total_pause_time);
    }
    
    return elapsed;
}

int timer_is_time_up(Timer *timer) {
    if (timer->time_limit <= 0) {
        return 0; // 无时间限制
    }
    
    return timer_get_elapsed_seconds(timer) >= timer->time_limit;
}

void timer_display(Timer *timer, const char *player_name) {
    int elapsed = timer_get_elapsed_seconds(timer);
    int minutes = elapsed / 60;
    int seconds = elapsed % 60;
    
    // 保持原有的显示格式，确保displayinterface兼容
    printf("%s方: %02d:%02d", player_name, minutes, seconds);
    
    if (timer->time_limit > 0) {
        int remaining = timer->time_limit - elapsed;
        if (remaining < 0) remaining = 0;
        int rem_minutes = remaining / 60;
        int rem_seconds = remaining % 60;
        printf(" (剩余: %02d:%02d)", rem_minutes, rem_seconds);
        
        if (timer_is_time_up(timer)) {
            printf(" ⏰ 超时!");
        }
    }
    
    // 添加状态指示器（不影响原有显示）
    if (timer->is_paused) {
        printf(" ⏸️");
    } else if (timer->is_running) {
        printf(" ▶️");
    } else {
        printf(" ⏹️");
    }
    
    printf("\n");
}

// 新增功能函数（game文件不需要调用这些，但可以在需要时使用）
void timer_start(Timer *timer) {
    if (!timer->is_running && !timer->is_paused) {
        timer->start_time = time(NULL);
        timer->is_running = true;
        timer->is_paused = false;
        timer->total_pause_time = 0;
    }
}

void timer_pause(Timer *timer) {
    if (timer->is_running && !timer->is_paused) {
        timer->pause_start = time(NULL);
        timer->is_paused = true;
    }
}

void timer_resume(Timer *timer) {
    if (timer->is_running && timer->is_paused) {
        time_t current_time = time(NULL);
        timer->total_pause_time += (current_time - timer->pause_start);
        timer->is_paused = false;
        timer->pause_start = 0;
    }
}

void timer_stop(Timer *timer) {
    if (timer->is_running) {
        if (timer->is_paused) {
            time_t current_time = time(NULL);
            timer->total_pause_time += (current_time - timer->pause_start);
        } else {
            time_t current_time = time(NULL);
            timer->elapsed_time += (current_time - timer->start_time - timer->total_pause_time);
        }
        
        timer->is_running = false;
        timer->is_paused = false;
        timer->start_time = 0;
        timer->pause_start = 0;
        timer->total_pause_time = 0;
    }
}

void timer_reset(Timer *timer) {
    timer->start_time = 0;
    timer->elapsed_time = 0;
    timer->is_running = false;
    timer->is_paused = false;
    timer->pause_start = 0;
    timer->total_pause_time = 0;
}

int timer_get_remaining_seconds(Timer *timer) {
    if (timer->time_limit <= 0) {
        return -1; // 无时间限制
    }
    
    int elapsed = timer_get_elapsed_seconds(timer);
    int remaining = timer->time_limit - elapsed;
    
    return (remaining > 0) ? remaining : 0;
}

void timer_format_time(time_t seconds, char *buffer, size_t buffer_size) {
    if (seconds < 0) {
        snprintf(buffer, buffer_size, "无限");
        return;
    }
    
    int minutes = seconds / 60;
    int secs = seconds % 60;
    
    if (minutes > 0) {
        snprintf(buffer, buffer_size, "%02d:%02d", minutes, secs);
    } else {
        snprintf(buffer, buffer_size, "%02d秒", secs);
    }
}

const char* timer_get_status_string(Timer *timer) {
    if (timer->is_paused) {
        return "暂停";
    } else if (timer->is_running) {
        return "运行中";
    } else {
        return "停止";
    }
}
