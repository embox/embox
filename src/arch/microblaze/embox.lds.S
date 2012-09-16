
#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

OUTPUT_FORMAT("elf32-microblaze", "elf32-microblaze", "elf32-microblaze-little")
OUTPUT_ARCH(microblaze)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

SECTIONS {
	.text : {
		ALIGNMENT();
		_ivt_start = .;
		*(.trap_table)
		_ivt_end = .;
		. = ALIGN(0x100);
		*(.text)
		*(.text.*)
		ALIGNMENT();
	} SECTION_REGION(text)
	SECTION_SYMBOLS(text)

	.rodata : {
		LDS_INPUT_RODATA
	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)

	.data : {
		ALIGNMENT();
		*(.data)
		ALIGNMENT();
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
		. += OPTION_GET(NUMBER,stack_size);
		ALIGNMENT();
		_stack = .;
	} SECTION_REGION(stack)
	SECTION_SYMBOLS(stack)
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;
