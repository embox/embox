
#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-powerpc", "elf32-powerpc", "elf32-powerpc")
OUTPUT_ARCH(powerpc)
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
}
