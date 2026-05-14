#include "sw_timer.h"

typedef unsigned long uintptr_t;

#define MTIME_ADDR      0x0200BFF8
#define MTIMECMP_ADDR   0x02004000

typedef struct {
	enum sw_timer_type_t;
	long ticks_period;
	long ticks;
	void (*callback)(const char*);
	bool is_active;
	unsigned long times_to_shot;
} sw_timer_t

static sw_timer_t timers[MAX_SOFT_TIMERS];
static uint64_t resolution = 0;
static volatile uint64_t * const mtime = (volatile uint64_t *)MTIME_ADDR;
static volatile uint64_t * const mtimecmp = (volatile uint64_t *)MTIMECMP_ADDR;

static int syscalls(unsigned long arg0, unsigned long arg1, 
    unsigned long arg2, unsigned long arg3, unsigned long arg4,
    unsigned long arg5, unsigned long arg6, unsigned long arg7) 
{
    // despite you don't need all these registers it is better to set them all
    // remember that in trap_vector in start.S you rewrite a0, a1 and a2 so you better to use another registers
    // for sending info to handle_trap

    register uintptr_t a0 asm("a0") = (uintptr_t)(arg0); // connect variable to register
    register uintptr_t a1 asm("a1") = (uintptr_t)(arg1); // connect variable to register
    register uintptr_t a2 asm("a2") = (uintptr_t)(arg2); // connect variable to register
    register uintptr_t a3 asm("a3") = (uintptr_t)(arg3); // connect variable to register
    register uintptr_t a4 asm("a4") = (uintptr_t)(arg4); // connect variable to register
    register uintptr_t a5 asm("a5") = (uintptr_t)(arg5); // connect variable to register
    register uintptr_t a6 asm("a6") = (uintptr_t)(arg6); // connect variable to register
    register uintptr_t a7 asm("a7") = (uintptr_t)(arg7); // connect variable to register

    asm volatile("ecall" 
        : "+r"(a0), "+r"(a1) 
        : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7) 
        : "memory");

    return a0;
}

int sw_timer_init(unsigned long resolution)
{

}

sw_timer_handle_t sw_timer_add(unsigned long ticks, sw_timer_type_t type, unsigned long times_to_shot, timer_callback_t callback_fun, void *arg)
{

}

bool sw_timer_is_active(sw_timer_handle_t index)
{

}

static void update_hardware_timer(void)
{

}

void sw_timer_irq_handler(void)
{

}
