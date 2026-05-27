OUTPUT_ARCH( "riscv" )
ENTRY( _start )

SECTIONS
{
  . = 0x80000000;
  
  .text : {
    KEEP(*(.text._start))
    *(.text)
  }

  .data   : { *(.data)   }
  .rodata : { *(.rodata) }
  .sdata  : { *(.sdata)  }

  . = ALIGN(16);
  stack_bottom = .;
  . += 0x1000;
  stack_top = .;

  _end = .;
}
