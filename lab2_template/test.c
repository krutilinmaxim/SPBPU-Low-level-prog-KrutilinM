typedef unsigned long uintptr_t;

#define UART_REG_TXFIFO 0x0

#define UART_BASE_ADDR 0x10010000
#define CLINT_BASE_ADDR 0x2000000
#define CLINT_MTIME_OFFSET 0xBFF8
#define CLINT_MTIMECMP_OFFSET 0x4000 // offset for hart #0

void end(void) asm("end");

static volatile int *mtime_addr = (int *)(void *)(CLINT_BASE_ADDR + CLINT_MTIME_OFFSET);
static volatile int *mtimecmp_addr = (int *)(void *)(CLINT_BASE_ADDR + CLINT_MTIMECMP_OFFSET);

static volatile int *uart = (int *)(void *)UART_BASE_ADDR;

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

static int putchar(char ch)
{
    while (uart[UART_REG_TXFIFO] < 0);
    return uart[UART_REG_TXFIFO] = ch & 0xFF;
}

void main(void)
{
    end();
}

long handle_trap(long cause, long epc, long regs[32])
{
    // step 1: determine if it is interrupt or exception
    // step 2:
    // if interrupt: check if it is timer interrupt then turn off machine timer interrupts in mie register
    // use timer_setup()
    // if unknown interrupt: send output with message about it and stop
    // if exception:
    // 1) parse info register to find out what is needed to be done
    // 2) if it is turning on timer interrupts, turn it on in mie register
    // use timer_setup()
    // 3) if it is console output, use putchar() function to output this symbol
    // 4) if unknown exception: send output with message about it and stop
    return epc;
}