typedef unsigned long uintptr_t;
typedef unsigned long long uint64;

#define UART_BASE_ADDR 0x10000000
#define CLINT_BASE_ADDR 0x02000000
#define CLINT_MTIME_OFFSET 0xBFF8
#define CLINT_MTIMECMP_OFFSET 0x4000

extern void timer_setup(unsigned long enable);
extern void end(void);

static volatile char *uart = (char *)UART_BASE_ADDR;
static volatile uint64 *mtime = (uint64 *)(CLINT_BASE_ADDR + CLINT_MTIME_OFFSET);
static volatile uint64 *mtimecmp = (uint64 *)(CLINT_BASE_ADDR + CLINT_MTIMECMP_OFFSET);

static void kernel_print(char *str) {
  while (*str) *uart = *str++;
}

static void syscall(uintptr_t id, uintptr_t arg) {
  register uintptr_t a3 asm("a3") = id;
  register uintptr_t a4 asm("a4") = arg;
  asm volatile("ecall" : : "r"(a3), "r"(a4) : "memory");
}

void main(void) {
  syscall(1, (uintptr_t)"Start. Krutilin Maxim.\n");
  syscall(2, 0);
  syscall(3, 5000000);
  syscall(4, 0);
  syscall(1, (uintptr_t)"Program finished execution.\n");
  end();
}

long handle_trap(uintptr_t cause, uintptr_t epc, uintptr_t *regs)
{
  uintptr_t code = cause & 0x3FF;
  
  // Р­РєСЃРµРїС€РµРЅС‹ (РїСЂРµСЂС‹РІР°РЅРёСЏ РѕС‚СЂРёС†Р°С‚РµР»СЊРЅС‹Рµ)
  if ((long)cause < 0) {
    if (code == 7) {
      timer_setup(0);
      kernel_print("HANDLER: Timer Interrupted\n");
    }
    goto done;
  }
  
  // РЎРёСЃС‚РµРјРЅС‹Рµ РІС‹Р·РѕРІС‹
  if (code == 8) {
    uintptr_t sys_id = regs[8];
    uintptr_t arg = regs[7];

    if (sys_id == 1) {
      kernel_print((char *)arg);
    }
    else if (sys_id == 2) {
      *mtimecmp = -1ULL;
      timer_setup(1);
      kernel_print("HANDLER: Timer Enabled (mtimecmp = max)\n");
    }
    else if (sys_id == 3) {
      *mtimecmp = *mtime + arg;
      kernel_print("HANDLER: Timer Set (mtime + arg)\n");
    }
    else if (sys_id == 4) {
      kernel_print("HANDLER: Entering WFI...\n");
      asm volatile("wfi");
    }
    else {
      kernel_print("Unknown Syscall\n");
    }
    
    return (long)epc + 4;
  }
  
  kernel_print("Unknown Exception Code\n");

done:
  return (long)epc;
}