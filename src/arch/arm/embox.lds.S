
#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-bigarm", "elf32-bigarm", "elf32-bigarm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

SECTIONS {
	.text : {
		*(.text)
	} SECTION_REGION(text)
	SECTION_SYMBOLS(text)

	.data : {
		_data_start = .;
		*(.data)
		_data_end = .;
	} SECTION_REGION(data)
	SECTION_SYMBOLS(data)

	.bss : {
		_bstart = .;
		*(.bss)
		_bend = .;
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(bss)
}

