
#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

SECTIONS {
	.text : {
		*(.trap_table)
		*(.text)
		*(.text.*)
		*(.glue_7t) *(.glue_7)
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
		*(.bss)
		ALIGNMENT();
		LDS_INPUT_RESERVE
	} SECTION_REGION(data)
	SECTION_SYMBOLS(data)

}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;
