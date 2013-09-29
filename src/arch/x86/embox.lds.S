
#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;

SECTIONS {
	.text : {
		*(.multiboot)
		_traps_text_start = .;
		*(.traps.*)
		_traps_text_end = .;
		*(.text)
		*(.text.*)
	}
}
