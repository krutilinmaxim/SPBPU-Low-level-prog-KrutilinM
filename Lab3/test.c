typedef unsigned long uintptr_t;
typedef unsigned long long uint64;

#define UART_ADDR 0x10000000
#define CLINT_BASE 0x02000000
#define CLINT_MTIME_OFFS 0xBFF8
#define CLINT_CMP_OFFS 0x4000

extern void enable_timer_interrupt(unsigned long enable);
extern void halt(void);

static volatile char* serial_port = (char*)UART_ADDR;
static volatile uint64* timer_mtime = (uint64*)(CLINT_BASE + CLINT_MTIME_OFFS);
static volatile uint64* timer_cmp = (uint64*)(CLINT_BASE + CLINT_CMP_OFFS);

static void serial_write(char* str) {
    while (*str)
        *serial_port = *str++;
}

static void do_syscall(uintptr_t id, uintptr_t arg) {
    register uintptr_t a3 asm("a3") = id;
    register uintptr_t a4 asm("a4") = arg;
    asm volatile("ecall" : : "r"(a3), "r"(a4) : "memory");
}

void main(void) {
    do_syscall(1, (uintptr_t)"Boot. Krutilin Maxim.\n");

    do_syscall(2, 0);
    do_syscall(3, 5000000);
    do_syscall(4, 0);
    do_syscall(1, (uintptr_t)"System finished.\n");
    halt();
}

long trap_dispatcher(uintptr_t cause, uintptr_t epc, uintptr_t* regs)
{
    uintptr_t code = cause & 0x3FF;

    if ((long)cause < 0) {
        if (code == 7) {
            enable_timer_interrupt(0);
            serial_write("TRAP: Timer event occurred\n");
        }
    }
    else {
        if (code == 8) {
            uintptr_t sys_id = regs[8];
            uintptr_t param = regs[7];

            switch (sys_id) {
            case 1:
                serial_write((char*)param);
                break;
            case 2:
                *timer_cmp = -1ULL;
                enable_timer_interrupt(1);
                serial_write("TRAP: Timer armed (max value)\n");
                break;
            case 3:
                *timer_cmp = *timer_mtime + param;
                serial_write("TRAP: Timer configured\n");
                break;
            case 4:
                serial_write("TRAP: Entering sleep...\n");
                asm volatile("wfi");
                break;
            default:
                serial_write("TRAP: Unknown request\n");
                break;
            }
            return (long)epc + 4;
        }
        else {
            serial_write("TRAP: Unknown exception\n");
        }
    }

    return (long)epc;
}