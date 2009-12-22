/*
 * This file is part of EMBOX.
 */

#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-microblaze", "elf32-microblaze", "elf32-microblaze")
OUTPUT_ARCH(microblaze)
ENTRY(_hardreset)

MEMORY {
	sdram : ORIGIN = LDS_RAM_BASE, LENGTH = LDS_RAM_SIZE
}

_stack = _bend + 0x200000;
__stack = _stack;
__checksum = 0x00000000;
_data_image_start = _data_lma;

_text_start = _text_vma;
_text_end = _text_vma + _text_len;

// XXX just for now. -- Eldar
//#ifdef DEBUG_TRG
//#define rom sdram
//#endif

SECTIONS {

	.text : {
		ALIGNMENT();
		*(.trap_table)
		. = ALIGN(0x100);
		*(.text)
		*(.init.text)
	} >sdram
	SECTION_SYMBOLS(text)

	.rodata (NOLOAD) : {
		LDS_INPUT_RODATA
		ALIGNMENT();
		_piggy_start = .;
		_piggy_end = .;
	} >sdram
	SECTION_SYMBOLS(rodata)

	.data : {
		ALIGNMENT();
		_data_start = .;
		*(.data)
		_data_end = .;
	} >sdram
	SECTION_SYMBOLS(data)

	.bss : {
		ALIGNMENT();
		_bstart = .;
		*(.bss)
		ALIGNMENT();
		_bend = ALIGN(0x8);
	} >sdram
	SECTION_SYMBOLS(bss)
}
