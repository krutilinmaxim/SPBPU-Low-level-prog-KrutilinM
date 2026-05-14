OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x80000000;

    .text.boot : {
        *(.text.boot)
        *(.text.timer_subsystem)
        *(.text)
        *(.text.*)
    }

    . = ALIGN(16);
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    . = ALIGN(16);
    .data : {
        *(.data.timer_pool)
        *(.data)
        *(.data.*)
    }

    . = ALIGN(16);
    .bss : {
        *(.bss)
        *(.bss.*)
    }

    /DISCARD/ : { *(.comment) *(.riscv.attributes) }
}