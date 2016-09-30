SECTIONS
{
  . = 0x0000;
  .boot : { *(.boot) }
  .text : { *(.text) }
  . = 0x40000;
  .data : { *(.data) }
  .bss : { *(.bss) }
}
