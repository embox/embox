
#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-microblaze", "elf32-microblaze", "elf32-microblaze-little")
OUTPUT_ARCH(microblaze)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;

SECTIONS {
	.text : {
		*(.trap_table.text)
		*(.text)
		*(.text.*)
	}

	_bstart = _bss_vma;
	_bend = _bstart + _bss_len;

}

