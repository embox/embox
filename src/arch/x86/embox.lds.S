
#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start)

MEMORY {
	MEMORY_REGION(ram)
}


_text_start = _text_vma;
_text_end = _text_vma + _text_len;
SECTIONS {
	//. = 0x100000;
	.text : {
		ALIGNMENT();
		*(.multiboot)
		*(.text)
		*(.init.text)
		ALIGNMENT();
	}  > ram
	SECTION_SYMBOLS(text)

	.rodata : {
		LDS_INPUT_RODATA
	} > ram
	SECTION_SYMBOLS(rodata)

	.data : {
		ALIGNMENT();
		_data_start = .;
		*(.data)
		ALIGNMENT();
		_data_end = .;
	} > ram
	SECTION_SYMBOLS(data)

	.bss : {
		ALIGNMENT();
		_bstart = .;
		*(.bss)
		ALIGNMENT();
		_bend = .;
	} > ram
	SECTION_SYMBOLS(bss)

	.stack (NOLOAD): {
		ALIGNMENT();
		_stack_bottom = .;
		. += CONFIG_STACK_SIZE;
		ALIGNMENT();
		_stack_top = .;
		_stack = _stack_top;
		__stack = _stack;
	} > ram
	/*
	/DISCARD/ : {
		*(.note)
		*(.note.ABI-tag)
		*(.eh_frame)
		*(.comment)
	}
	*/
}

