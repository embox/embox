
#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

OUTPUT_FORMAT("elf32-bigmips", "elf32-bigmips", "elf32-littlemips")

OUTPUT_ARCH(mips)

ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

_data_image_start = _data_lma;

_text_start = _text_vma;
_text_end = _text_vma + _text_len;

SECTIONS {
	.text : {
		. = + 0x400;
		ALIGNMENT();
		_ivt_start = .;
		*(.trap_table)
		_ivt_end = .;
		. = ALIGN(0x100);
		*(.text)
		*(.text.*)
		*(.init.text)
		ALIGNMENT();
	} SECTION_REGION(text)
	SECTION_SYMBOLS(text)

	.rodata : {
		LDS_INPUT_RODATA
	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)

	.data : {
		ALIGNMENT();
		_data_start = .;
		*(.data)
		*(.sdata)
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

	.reserve (NOLOAD): {
		LDS_INPUT_RESERVE
	/* TODO memory region for reserve output section. */
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(reserve)

	.stack (NOLOAD): {
		ALIGNMENT();
		_stack_bottom = .;
		. += OPTION_GET(NUMBER,stack_size);
		ALIGNMENT();
		_stack_top = .;
		_stack = _stack_top;
		__stack = _stack;
	} SECTION_REGION(stack)
	SECTION_SYMBOLS(stack)

		/* Sections to be discarded */
	/DISCARD/ : {
		*(.MIPS.options)
		*(.options)
		*(.pdr)
		*(.reginfo)
		*(.comment)
		*(.note)
	}
}

_free_mem = _stack_top;
_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;
