typedef unsigned long uintptr_t;
typedef unsigned long long uint64;

#define UART_ADDR 0x10000000
#define CLINT_ADDR 0x02000000
#define MTIME_OFF 0xBFF8
#define MTIMECMP_OFF 0x4000

extern void enable_timer(unsigned long flag);
extern void finish(void);

static volatile char* serial_port = (char*)UART_ADDR;
static volatile uint64* timer_mtime = (uint64*)(CLINT_ADDR + MTIME_OFF);
static volatile uint64* timer_mtimecmp = (uint64*)(CLINT_ADDR + MTIMECMP_OFF);

static void print_string(char* msg) {
    while (*msg) *serial_port = *msg++;
}

static void do_ecall(uintptr_t cmd, uintptr_t param) {
    register uintptr_t r3 asm("a3") = cmd;
    register uintptr_t r4 asm("a4") = param;
    asm volatile("ecall" : : "r"(r3), "r"(r4) : "memory");
}

void main(void) {
    do_ecall(1, (uintptr_t)"Start. Krutilin Maxim.\n");
    do_ecall(2, 0);
    do_ecall(3, 5000000);
    do_ecall(4, 0);
    do_ecall(1, (uintptr_t)"Program finished execution.\n");
    finish();
}

long trap_handler(uintptr_t reason, uintptr_t pc, uintptr_t* ctx)
{
    uintptr_t exc_code = reason & 0x3FF;

    // РџСЂРµСЂС‹РІР°РЅРёСЏ
    if ((long)reason < 0) {
        if (exc_code == 7) {
            enable_timer(0);
            print_string("HANDLER: Timer Interrupted\n");
        }
        return (long)pc;
    }

    // РЎРёСЃС‚РµРјРЅС‹Рµ РІС‹Р·РѕРІС‹
    if (exc_code == 8) {
        uintptr_t command = ctx[8];
        uintptr_t data = ctx[7];

        switch (command) {
        case 1:
            print_string((char*)data);
            break;
        case 2:
            *timer_mtimecmp = -1ULL;
            enable_timer(1);
            print_string("HANDLER: Timer Enabled (mtimecmp = max)\n");
            break;
        case 3:
            *timer_mtimecmp = *timer_mtime + data;
            print_string("HANDLER: Timer Set (mtime + arg)\n");
            break;
        case 4:
            print_string("HANDLER: Entering WFI...\n");
            asm volatile("wfi");
            break;
        default:
            print_string("Unknown Syscall\n");
        }
        return (long)pc + 4;
    }

    print_string("Unknown Exception Code\n");
    return (long)pc;
}