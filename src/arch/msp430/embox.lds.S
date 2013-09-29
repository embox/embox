
#include <asm-generic/embox.lds.h>

ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
	vectors          : ORIGIN = 0xffe0, LENGTH = 0x0020
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;

SECTIONS {
	.vectors : {
		*(.vectors)
	} > vectors

	.text : {
		*(.text)
		*(.text.*)
		*(.glue_7t) *(.glue_7)
	}
}
