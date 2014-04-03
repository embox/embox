
#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-microblaze", "elf32-microblaze", "elf32-microblaze-little")
OUTPUT_ARCH(microblaze)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(RAM)
	MEMORY_REGION(ROM)
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		*(.trap_table.text)
		*(.text)
		*(.text.*)
	}

	_bstart = _bss_vma;
	_bend = _bstart + _bss_len;

}

