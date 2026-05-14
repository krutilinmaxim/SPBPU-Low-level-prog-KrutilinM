OUTPUT_ARCH( "riscv" )
ENTRY( _start )
SECTIONS
{
  START_ADDRESS = 0x80000000;
  STACK_SIZE = 0x800;
  
  /* start addresss */
  . = START_ADDRESS;

  /* text */
  .text : { *(.text) }
  
  /* data */
  .data : { *(.data) }
  .rodata : { *(.rodata) }
  .sdata : { *(.sdata) }
  .debug : { *(.debug) }

  /* stack */
  stack_bottom =.;
  . += STACK_SIZE;
  stack_top = .;

  _end = .;
}