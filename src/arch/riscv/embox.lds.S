#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)

ENTRY(_start)

MEMORY {
	MEMORY_REGION(RAM)
	MEMORY_REGION(ROM)
}

PROVIDE(_ram_base = ORIGIN(RAM));
PROVIDE(_ram_size = LENGTH(RAM));

SECTIONS {
	.text : {
		*(.text.init_start)
		*(.text .text.*)
	}

	.rodata : {
		*(.rodata .rodata.*)

	} >ROM AT>ROM :text

	.data : {
		. = ALIGN(4096);
		PROVIDE( __global_pointer$ = . + 0x800 );
		*(.sdata .sdata.*) *(.data .data.*)

	} >RAM AT>RAM :data

	.bss :{
		. = ALIGN(32);
		PROVIDE( bss_target_start = . );
		*(.sbss .sbss.*) *(.bss .bss.*)
		PROVIDE( bss_target_end = . );

	} >RAM AT>RAM :bss
}
