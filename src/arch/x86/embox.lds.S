
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
	. = ALIGN(0x1000);
	__KERNEL_START = .;

	.text : {
		ALIGNMENT();
		. = ALIGN(0x100);
		*(.multiboot)
		*(.text)
		*(.text.*)
		ALIGNMENT();
		. = ALIGN(0x100);
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
	} SECTION_REGION(stack)
	SECTION_SYMBOLS(stack)

	KERNEL_END = .;
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;
