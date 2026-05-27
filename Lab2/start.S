.section .text._start
.global _start
.global timer_setup
.global end

_start:
    csrr    t0, mhartid
    bnez    t0, park

    # Регистрация trap_vector
    la      t1, trap_vector
    csrw    mtvec, t1
    li      t1, 0x1800
    csrc    mstatus, t1
    csrsi   mstatus, (0x1 << 3)

    # Настройка PMP
    li      t1, -1
    csrw    pmpaddr0, t1
    li      t1, 0x1F
    csrw    pmpcfg0, t1
    la      sp, stack_top

    # Установка адреса возврата в main и переход через mret
    la      t2, main
    csrw    mepc, t2
    mret

timer_setup:
    li      t0, (1 << 7)
    beqz    a0, .Ldisable
    csrs    mie, t0
    ret
.Ldisable:
    csrc    mie, t0
    ret

park:
    wfi
    j       park

end:
    j       end

.align 4
trap_vector:
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
    csrr    a1, mepc      # Передаем адрес инструкции
    mv      a2, sp        # Передаем указатель на стек

    call    handle_trap

    # Устанавливаем новый адрес возврата из результата Си
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
