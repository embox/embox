
#include <asm-generic/embox.lds.h>

/*OUTPUT_FORMAT("elf32-rc", "elf32-sparc", "elf32-sparc") */
OUTPUT_ARCH(arm)
ENTRY(start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
	/*MEMORY_REGION(internal) */
}

/*_stack = _bend + 0x10000;*/
/*__stack = _stack;*/
/*__checksum = 0x00000000;*/
_data_image_start = _data_lma;

_text_start = _text_vma;
_text_end = _text_vma + _text_len;
/*_reloc_end = _reloc_start + _reloc_len; */
SECTIONS {
	.text : {
		*(.trap_table)
		*(.text)
		*(.init.text)
		ALIGNMENT();
		*(.eh_frame)
		ALIGNMENT();
	} SECTION_REGION(text)
	SECTION_SYMBOLS(text)

	.reloc : {
		ALIGNMENT();
		_reloc_text_start = .;
		*(.reloc.text)
		ALIGNMENT();
		_reloc_text_end = .;
	} SECTION_REGION(reloc)
	SECTION_SYMBOLS(reloc)

	.rodata (NOLOAD) : {
		LDS_INPUT_RODATA
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
}
