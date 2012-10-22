
#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

SECTIONS {
	.text : {
		ALIGNMENT(); . = ALIGN(0x1000);
		*(.multiboot)
		_traps_text_start = .;
		*(.traps.*)
		_traps_text_end = .;
		*(.text)
		*(.text.*)
		ALIGNMENT(); . = ALIGN(0x1000);
	} SECTION_REGION(text)
	SECTION_SYMBOLS(text)

	.rodata : {
		ALIGNMENT(); . = ALIGN(0x1000);
		LDS_INPUT_RODATA
		ALIGNMENT(); . = ALIGN(0x1000);
	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)

	.data : {
		ALIGNMENT(); . = ALIGN(0x1000);
		*(.data)
		ALIGNMENT(); . = ALIGN(0x1000);
	} SECTION_REGION(data)
	SECTION_SYMBOLS(data)

	.bss : {
		ALIGNMENT(); . = ALIGN(0x1000);
		*(.bss)
		ALIGNMENT(); . = ALIGN(0x1000);
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(bss)

	.reserve (NOLOAD): {
		ALIGNMENT(); . = ALIGN(0x1000);
		LDS_INPUT_RESERVE
	/* TODO memory region for reserve output section. */
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(reserve)
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;
