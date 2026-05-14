.section .text.boot
.global _start
.global enable_timer_interrupt
.global halt

_start:
    csrr    t0, mhartid
    bnez    t0, idle_loop

    la      t1, exception_handler
    csrw    mtvec, t1

    # ---------- PMP configuration ----------
    # Region 0: CLINT protection (NAPOT, no access)
    li      t1, 0x00801FFF
    csrw    pmpaddr0, t1

    # Region 1: Full memory access (NAPOT, RWX)
    li      t1, -1
    srli    t1, t1, 2
    csrw    pmpaddr1, t1

    # pmpcfg0: region0=0x18, region1=0x1F
    li      t1, 0x1F18
    csrw    pmpcfg0, t1
    # ---------------------------------------

    # Switch to U-mode with interrupts enabled
    li      t1, 0x1800
    csrc    mstatus, t1
    li      t1, (1 << 7) | (1 << 3)
    csrs    mstatus, t1

    la      sp, stack_top
    la      t2, main
    csrw    mepc, t2
    mret

enable_timer_interrupt:
    li      t0, (1 << 7)
    beqz    a0, .disable_timer
    csrs    mie, t0
    ret
.disable_timer:
    csrc    mie, t0
    ret

idle_loop:
    wfi
    j       idle_loop

halt:
    j       halt

.align 4
exception_handler:
    addi    sp, sp, -16*8
    sd      x1,  15*8(sp)
    sd      x5,  14*8(sp)
    sd      x6,  13*8(sp)
    sd      x7,  12*8(sp)
    sd      x10, 11*8(sp)
    sd      x11, 10*8(sp)
    sd      x12, 9*8(sp)
    sd      x13, 8*8(sp)
    sd      x14, 7*8(sp)
    sd      x15, 6*8(sp)
    sd      x16, 5*8(sp)
    sd      x17, 4*8(sp)
    sd      x28, 3*8(sp)
    sd      x29, 2*8(sp)
    sd      x30, 1*8(sp)
    sd      x31, 0*8(sp)

    csrr    a0, mcause
    csrr    a1, mepc
    mv      a2, sp

    call    trap_dispatcher

    csrw    mepc, a0

    ld      x1,  15*8(sp)
    ld      x5,  14*8(sp)
    ld      x6,  13*8(sp)
    ld      x7,  12*8(sp)
    ld      x10, 11*8(sp)
    ld      x11, 10*8(sp)
    ld      x12, 9*8(sp)
    ld      x13, 8*8(sp)
    ld      x14, 7*8(sp)
    ld      x15, 6*8(sp)
    ld      x16, 5*8(sp)
    ld      x17, 4*8(sp)
    ld      x28, 3*8(sp)
    ld      x29, 2*8(sp)
    ld      x30, 1*8(sp)
    ld      x31, 0*8(sp)
    addi    sp, sp, 16*8
    mret

.section .data
.align 16
stack_space: .space 4096
stack_top: