
#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-microblaze", "elf32-microblaze", "elf32-microblaze")
OUTPUT_ARCH(microblaze)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}
/*
_stack = _bend + 0x200000;
__stack = _stack;
__checksum = 0x00000000;*/
_data_image_start = _data_lma;

_text_start = _text_vma;
_text_end = _text_vma + _text_len;

SECTIONS {

	.text : {
		ALIGNMENT();
		*(.trap_table)
		. = ALIGN(0x100);
		*(.text)
		*(.init.text)
		ALIGNMENT();
	} SECTION_REGION(text)
	SECTION_SYMBOLS(text)

	.rodata (NOLOAD) : {
		LDS_INPUT_RODATA
		ALIGNMENT();
		_ramfs_start = .;
		*(.ramfs)
		_ramfs_end = .;
	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)

	.data : {
		ALIGNMENT();
		_data_start = .;
		*(.data)
		ALIGNMENT();
		_data_end = .;
	} SECTION_REGION(data)
	SECTION_SYMBOLS(data)

	.bss : {
		ALIGNMENT();
		_bstart = .;
		*(.bss)
		*(.sbss)
		ALIGNMENT();
		_bend = .;
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(bss)

	.stack (NOLOAD): {
		ALIGNMENT();
		_stack_bottom = .;
		. += CONFIG_STACK_SIZE;
		ALIGNMENT();
		_stack_top = .;
		_stack = _stack_top;
		__stack = _stack;
	} SECTION_REGION(stack)
	SECTION_SYMBOLS(stack)
}
