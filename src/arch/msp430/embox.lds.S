
#include <asm-generic/embox.lds.h>

ENTRY(_start)

MEMORY {
	MEMORY_REGION(RAM)
	MEMORY_REGION(ROM)
	vectors          : ORIGIN = 0xffe0, LENGTH = 0x0020
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.vectors : {
		KEEP(*(.vectors));
	} > vectors

	.text : {
		*(.text)
		*(.text.*)
		*(.glue_7t) *(.glue_7)
	} SECTION_REGION(text) :text
}
