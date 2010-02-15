/*
 * This file is part of EMBOX.
 */

#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-sparc", "elf32-sparc", "elf32-sparc")
OUTPUT_ARCH(sparc)
ENTRY(start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
	MEMORY_REGION(internal)
}

_internal_stack = ORIGIN(internal) + LENGTH(internal);
_stack = _bend + 0x200000;
__stack = _stack;
__checksum = 0x00000000;
_data_image_start = _data_lma;

_text_start = _text_vma;
_text_end = _text_vma + _text_len;

SECTIONS {

	.text : {
		ALIGNMENT();
		*(.trap_table.text)
		*(.text)
		*(.init.text)
	} SECTION_REGION(text)
	SECTION_SYMBOLS(text)

	.rodata (NOLOAD) : {
		LDS_INPUT_RODATA
		ALIGNMENT();
		_piggy_start = .;
		PIGGY
		_piggy_end = .;
		ALIGNMENT();
	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)

	.data : {
		ALIGNMENT();
		_data_start = .;
		*(.data)
		_data_end = .;
	} SECTION_REGION(data)
	SECTION_SYMBOLS(data)

	.bss : {
		ALIGNMENT();
		_bstart = .;
		*(.bss)
		ALIGNMENT();
		_bend = ALIGN(0x8);
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(bss)

	.internal_mem (NOLOAD) : {
		. = ALIGN(0x8);
		_internal_start = .;
		*(.internal_mem)
		_internal_end = .;
	} SECTION_REGION(internal_mem)

}
