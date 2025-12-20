#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <stdbool.h>

// 保持原有的结构体定义不变，确保向后兼容
typedef struct {
    time_t start_time;      // 计时开始时间
    time_t elapsed_time;    // 已用时间（秒）
    time_t time_limit;      // 时间限制（秒），0表示无限制
    bool is_running;        // 是否正在运行
    bool is_paused;         // 是否暂停
    time_t pause_start;     // 暂停开始时间
    time_t total_pause_time; // 总暂停时间
} Timer;

// 保持原有的函数声明不变（向后兼容）
void timer_init(Timer *timer, int time_limit);
int timer_get_elapsed_seconds(Timer *timer);
int timer_is_time_up(Timer *timer);
void timer_display(Timer *timer, const char *player_name);

// 新增功能函数（但game和displayinterface不需要调用这些）
void timer_start(Timer *timer);
void timer_pause(Timer *timer);
void timer_resume(Timer *timer);
void timer_stop(Timer *timer);
void timer_reset(Timer *timer);
int timer_get_remaining_seconds(Timer *timer);
void timer_format_time(time_t seconds, char *buffer, size_t buffer_size);
const char* timer_get_status_string(Timer *timer);

#endif
