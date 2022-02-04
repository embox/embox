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
		KEEP(*(.init_start));
		*(.text)
		*(.text.*)
	}

	.global_pointers : {
		. = ALIGN(4096);
		PROVIDE( __global_pointer$ = . + 0x800 );
	} SECTION_REGION(data)
/*
	.data : {
		. = ALIGN(4096);
		PROVIDE( __global_pointer$ = . + 0x800 );

		*(.sdata .sdata.*) *(.data .data.*)

	} SECTION_REGION(data)
*/}
