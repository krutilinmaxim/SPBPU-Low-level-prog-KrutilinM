#include "sw_timer.h"

#define CONSOLE_BASE       0x10010000
#define CONSOLE_TX_REG     0x0

static volatile int* console_tx = (int*)(void*)CONSOLE_BASE;

static void console_write_char(char ch) {
    while (console_tx[CONSOLE_TX_REG] < 0);
    console_tx[CONSOLE_TX_REG] = ch & 0xFF;
}

static void console_print(char* message) {
    while (*message)
        console_write_char(*message++);
}

static void event_handler(void* payload) {
    console_print((char*)payload);
}

void main(void) {
    console_print("System starting in U-mode!\n");

    timer_subsystem_init(500000);

    timer_create(2, TIMER_ONE_SHOT, 0, event_handler, "[ONE-SHOT] Event triggered!\n");
    timer_create(3, TIMER_BURST, 5, event_handler, "[BURST] Tick\n");
    timer_create(4, TIMER_INFINITE, 0, event_handler, "[INFINITE] Heartbeat\n");

    while (1) {
        asm volatile("wfi");
    }
}