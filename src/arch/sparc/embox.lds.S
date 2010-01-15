/*
 * This file is part of EMBOX.
 */

#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-sparc", "elf32-sparc", "elf32-sparc")
OUTPUT_ARCH(sparc)
ENTRY(_hardreset)

MEMORY {
	LDS_REGION_SDRAM
	LDS_REGION_FLASH
	LDS_REGION_INTERNAL
}

_internal_stack = ORIGIN(REGION_INTERNAL) + LENGTH(REGION_INTERNAL);
_stack = _bend + 0x200000;
__stack = _stack;
__checksum = 0x00000000;
_data_image_start = _data_lma;

_text_start = _text_vma;
_text_end = _text_vma + _text_len;

SECTIONS {

	.text : {
		ALIGNMENT();
		*(.trap_table)
		*(.text)
		*(.init.text)
	} >PLACE_TEXT
	SECTION_SYMBOLS(text)

	.rodata (NOLOAD) : {
		LDS_INPUT_RODATA
		ALIGNMENT();
		_piggy_start = .;
		_piggy_end = .;
	} >PLACE_RODATA
	SECTION_SYMBOLS(rodata)

	.data : LMA_DATA {
		ALIGNMENT();
		_data_start = .;
		*(.data)
		_data_end = .;
	} >PLACE_DATA
	SECTION_SYMBOLS(data)

	.bss : {
		ALIGNMENT();
		_bstart = .;
		*(.bss)
		ALIGNMENT();
		_bend = ALIGN(0x8);
	}  >PLACE_BSS
	SECTION_SYMBOLS(bss)

	.internal_mem (NOLOAD) : {
		. = ALIGN(0x8);
		_internal_start = .;
		*(.internal_mem)
		_internal_end = .;
	}  >PLACE_INTERNAL
}
