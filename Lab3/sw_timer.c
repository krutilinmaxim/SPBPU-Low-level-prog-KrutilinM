#include "sw_timer.h"

#define CLINT_MTIME_ADDR    0x0200BFF8
#define CLINT_MTIMECMP_ADDR 0x02004000

extern void enable_timer_interrupt(unsigned long enable);

#ifndef MAX_SOFT_TIMERS
#error "MAX_SOFT_TIMERS not defined"
#endif

typedef struct {
    timer_mode_t mode;
    unsigned long period_ticks;
    unsigned long remaining_ticks;
    callback_fn_t callback;
    void* user_arg;
    bool active;
    unsigned long remaining_shots;
} timer_entry_t;

static timer_entry_t timer_pool[MAX_SOFT_TIMERS] __attribute__((section(".data.timer_pool")));
static unsigned long base_resolution __attribute__((section(".data.timer_pool")));
static volatile uint64_t* const __attribute__((section(".data.timer_pool"))) clint_mtime = (volatile uint64_t*)CLINT_MTIME_ADDR;
static volatile uint64_t* const __attribute__((section(".data.timer_pool"))) clint_mtimecmp = (volatile uint64_t*)CLINT_MTIMECMP_ADDR;

static int invoke_service(unsigned long service_id, unsigned long param0, unsigned long param1) {
    register uintptr_t a3 asm("a3") = (uintptr_t)service_id;
    register uintptr_t a4 asm("a4") = (uintptr_t)param0;
    register uintptr_t a5 asm("a5") = (uintptr_t)param1;
    asm volatile("ecall" : : "r"(a3), "r"(a4), "r"(a5) : "memory");
    return 0;
}

int __attribute__((section(".text.timer_subsystem"))) timer_subsystem_init(unsigned long res) {
    base_resolution = res;
    for (int i = 0; i < MAX_SOFT_TIMERS; i++)
        timer_pool[i].active = false;
    return 0;
}

timer_id_t __attribute__((section(".text.timer_subsystem")))
timer_create(unsigned long delay_ticks, timer_mode_t mode,
    unsigned long repeat_count, callback_fn_t callback, void* user_data) {
    timer_id_t id = -1;
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (!timer_pool[i].active) {
            id = i;
            break;
        }
    }
    if (id == -1) return -1;

    timer_pool[id].mode = mode;
    timer_pool[id].period_ticks = delay_ticks;
    timer_pool[id].remaining_ticks = delay_ticks;
    timer_pool[id].callback = callback;
    timer_pool[id].user_arg = user_data;
    timer_pool[id].active = true;
    timer_pool[id].remaining_shots = (mode == TIMER_BURST) ? repeat_count : 0;

    invoke_service(2, 0, 0);
    return id;
}

bool __attribute__((section(".text.timer_subsystem")))
timer_is_running(timer_id_t id) {
    if (id < 0 || id >= MAX_SOFT_TIMERS) return false;
    return timer_pool[id].active;
}

int __attribute__((section(".text.timer_subsystem")))
timer_cancel(timer_id_t id) {
    if (id < 0 || id >= MAX_SOFT_TIMERS) return -1;
    if (!timer_pool[id].active) return -1;
    timer_pool[id].active = false;
    invoke_service(5, 0, 0);
    return 0;
}

int __attribute__((section(".text.timer_subsystem")))
timer_modify(timer_id_t id, unsigned long new_delay_ticks) {
    if (id < 0 || id >= MAX_SOFT_TIMERS) return -1;
    if (!timer_pool[id].active) return -1;
    timer_pool[id].period_ticks = new_delay_ticks;
    timer_pool[id].remaining_ticks = new_delay_ticks;
    invoke_service(5, 0, 0);
    return 0;
}

static void __attribute__((section(".text.timer_subsystem")))
refresh_hardware_timer(void) {
    int has_active_timers = 0;
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (timer_pool[i].active) {
            has_active_timers = 1;
            break;
        }
    }
    if (has_active_timers) {
        *clint_mtimecmp = *clint_mtime + base_resolution;
        enable_timer_interrupt(1);
    }
    else {
        enable_timer_interrupt(0);
    }
}

void __attribute__((section(".text.timer_subsystem")))
timer_interrupt_handler(void) {
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (!timer_pool[i].active) continue;
        if (timer_pool[i].remaining_ticks > 0) timer_pool[i].remaining_ticks--;

        if (timer_pool[i].remaining_ticks == 0) {
            if (timer_pool[i].callback) timer_pool[i].callback(timer_pool[i].user_arg);

            switch (timer_pool[i].mode) {
            case TIMER_ONE_SHOT:
                timer_pool[i].active = false;
                break;
            case TIMER_BURST:
                if (timer_pool[i].remaining_shots > 1) {
                    timer_pool[i].remaining_shots--;
                    timer_pool[i].remaining_ticks = timer_pool[i].period_ticks;
                }
                else {
                    timer_pool[i].active = false;
                }
                break;
            case TIMER_INFINITE:
                timer_pool[i].remaining_ticks = timer_pool[i].period_ticks;
                break;
            }
        }
    }
    refresh_hardware_timer();
}

long __attribute__((section(".text.timer_subsystem")))
trap_dispatcher(unsigned long cause, unsigned long epc, unsigned long* regs) {
    unsigned long exc_code = cause & 0x3FF;

    if ((long)cause < 0) {
        if (exc_code == 7) timer_interrupt_handler();
        return (long)epc;
    }
    else {
        if (exc_code == 8) {
            unsigned long svc_id = regs[8];
            switch (svc_id) {
            case 2:
                *clint_mtimecmp = *clint_mtime + base_resolution;
                enable_timer_interrupt(1);
                break;
            case 5:
                refresh_hardware_timer();
                break;
            default:
                break;
            }
            return (long)epc + 4;
        }
        return (long)epc;
    }
}