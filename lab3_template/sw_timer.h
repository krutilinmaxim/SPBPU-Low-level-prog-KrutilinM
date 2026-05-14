#ifndef SW_TIMER_H
#define SW_TIMER_H

typedef void (*timer_callback_t)(void *arg);

typedef int sw_timer_handle_t;

typedef enum {
	ONESHOT,
	MULTISHOT,
	PERIODIC
} sw_timer_type_t;

int sw_timer_init(unsigned long resolution);

sw_timer_handle_t sw_timer_add(unsigned long ticks, sw_timer_type_t type, unsigned long times_to_shot, timer_callback_t callback_fun, void *arg);

bool sw_timer_is_active(sw_timer_handle_t index);

#endif