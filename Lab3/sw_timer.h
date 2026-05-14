#ifndef SOFT_TIMER_MODULE_H
#define SOFT_TIMER_MODULE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TIMER_ONE_SHOT,
    TIMER_BURST,
    TIMER_INFINITE
} timer_mode_t;

typedef int timer_id_t;
typedef void (*callback_fn_t)(void* user_data);

int timer_subsystem_init(unsigned long tick_resolution);
timer_id_t timer_create(unsigned long delay_ticks, timer_mode_t mode,
    unsigned long repeat_count, callback_fn_t callback, void* user_data);
bool timer_is_running(timer_id_t id);
int timer_cancel(timer_id_t id);
int timer_modify(timer_id_t id, unsigned long new_delay_ticks);
long trap_dispatcher(unsigned long cause, unsigned long epc, unsigned long* regs);

#endif