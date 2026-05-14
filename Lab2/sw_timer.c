#include "sw_timer.h"

typedef unsigned long uintptr_t;

#define TIMER_MTIME_ADDR    0x0200BFF8
#define TIMER_MTIMECMP_ADDR 0x02004000

typedef struct {
  timer_kind_t kind;
  long period_ticks;
  long remaining_ticks;
  callback_func_t callback;
  int is_running;
  unsigned long remaining_shots;
} soft_timer_entry_t;

static soft_timer_entry_t timer_pool[MAX_SOFT_TIMERS];
static uint64_t tick_resolution = 0;
static volatile uint64_t * const global_mtime = (volatile uint64_t *)TIMER_MTIME_ADDR;
static volatile uint64_t * const global_mtimecmp = (volatile uint64_t *)TIMER_MTIMECMP_ADDR;

static int invoke_syscall(unsigned long a0, unsigned long a1, 
    unsigned long a2, unsigned long a3, unsigned long a4,
    unsigned long a5, unsigned long a6, unsigned long a7) 
{
    register uintptr_t r0 asm("a0") = (uintptr_t)a0;
    register uintptr_t r1 asm("a1") = (uintptr_t)a1;
    register uintptr_t r2 asm("a2") = (uintptr_t)a2;
    register uintptr_t r3 asm("a3") = (uintptr_t)a3;
    register uintptr_t r4 asm("a4") = (uintptr_t)a4;
    register uintptr_t r5 asm("a5") = (uintptr_t)a5;
    register uintptr_t r6 asm("a6") = (uintptr_t)a6;
    register uintptr_t r7 asm("a7") = (uintptr_t)a7;

    asm volatile("ecall" 
        : "+r"(r0), "+r"(r1) 
        : "r"(r2), "r"(r3), "r"(r4), "r"(r5), "r"(r6), "r"(r7) 
        : "memory");

    return r0;
}

int init_soft_timer(unsigned long resolution)
{
  if (resolution == 0) return -1;
  tick_resolution = resolution;
  for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
    timer_pool[i].is_running = 0;
  }
  return 0;
}

timer_handle_t create_soft_timer(unsigned long ticks, timer_kind_t kind, 
                                  unsigned long repeat_count, callback_func_t callback, void *argument)
{
  for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
    if (!timer_pool[i].is_running) {
      timer_pool[i].kind = kind;
      timer_pool[i].period_ticks = ticks;
      timer_pool[i].remaining_ticks = ticks;
      timer_pool[i].callback = callback;
      timer_pool[i].is_running = 1;
      timer_pool[i].remaining_shots = repeat_count;
      return i;
    }
  }
  return -1;
}

int is_soft_timer_active(timer_handle_t idx)
{
  if (idx < 0 || idx >= MAX_SOFT_TIMERS) return 0;
  return timer_pool[idx].is_running;
}

static void refresh_hw_timer(void)
{
  uint64_t soonest = ~0ULL;
  int target_idx = -1;
  
  for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
    if (timer_pool[i].is_running && timer_pool[i].remaining_ticks < soonest) {
      soonest = timer_pool[i].remaining_ticks;
      target_idx = i;
    }
  }
  
  if (target_idx >= 0) {
    *global_mtimecmp = *global_mtime + timer_pool[target_idx].remaining_ticks * tick_resolution;
  }
}

void soft_timer_interrupt_handler(void)
{
  uint64_t current = *global_mtime;
  
  for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
    if (!timer_pool[i].is_running) continue;
    
    timer_pool[i].remaining_ticks--;
    
    if (timer_pool[i].remaining_ticks == 0) {
      if (timer_pool[i].callback) {
        timer_pool[i].callback(0);
      }
      
      if (timer_pool[i].kind == SINGLE_SHOT) {
        timer_pool[i].is_running = 0;
      }
      else if (timer_pool[i].kind == MULTI_SHOT) {
        timer_pool[i].remaining_shots--;
        if (timer_pool[i].remaining_shots == 0) {
          timer_pool[i].is_running = 0;
        } else {
          timer_pool[i].remaining_ticks = timer_pool[i].period_ticks;
        }
      }
      else if (timer_pool[i].kind == REPEATING) {
        timer_pool[i].remaining_ticks = timer_pool[i].period_ticks;
      }
    }
  }
  
  refresh_hw_timer();
}