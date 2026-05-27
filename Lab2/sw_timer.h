#ifndef SW_TIMER_H
#define SW_TIMER_H
#define MAX_SOFT_TIMERS 8

typedef void (*callback_func_t)(void* argument);

typedef int timer_handle_t;

typedef enum {
    SINGLE_SHOT,
    MULTI_SHOT,
    REPEATING
} timer_kind_t;

int init_soft_timer(unsigned long resolution);

timer_handle_t create_soft_timer(unsigned long ticks, timer_kind_t kind,
    unsigned long repeat_count, callback_func_t callback, void* argument);

int is_soft_timer_active(timer_handle_t idx);

#endif