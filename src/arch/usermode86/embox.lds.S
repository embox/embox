
#include <asm-generic/embox.lds.h>

ENTRY(_start)

MEMORY {
	MEMORY_REGION(RAM)
	MEMORY_REGION(ROM)
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		*(.trap_table)
		*(.text)
		*(.text.*)
		*(.glue_7t) *(.glue_7)
	}
}
