
#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;

SECTIONS {
	.text : {
		*(.trap_table)
		*(.text)
		*(.text.*)
		*(.glue_7t) *(.glue_7)
	}
}
