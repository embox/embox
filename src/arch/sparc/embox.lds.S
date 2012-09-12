
#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

OUTPUT_FORMAT("elf32-sparc", "elf32-sparc", "elf32-sparc")
OUTPUT_ARCH(sparc)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

SECTIONS {

	.text : {
		*(.trap_table.text)
		*(.text)
		*(.text.*)
	} SECTION_REGION(text)
	SECTION_SYMBOLS(text)

	.rodata : {
		LDS_INPUT_RODATA
	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)

	.data : {
		ALIGNMENT();
		*(.data)
	} SECTION_REGION(data)
	SECTION_SYMBOLS(data)

	.bss : {
		ALIGNMENT();
		*(.bss)
		ALIGNMENT();
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(bss)

	.reserve (NOLOAD): {
		LDS_INPUT_RESERVE
	/* TODO memory region for reserve output section. */
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(reserve)

	.stack (NOLOAD): {
		ALIGNMENT();
		. += OPTION_GET(NUMBER,stack_size);
		ALIGNMENT();
		__stack = .;
	} SECTION_REGION(stack)
	SECTION_SYMBOLS(stack)
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;
