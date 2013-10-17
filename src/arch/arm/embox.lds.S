
#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(start)

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

	/* XXX symbols __exidx_start/end are required by libgcc.a --Alexander */
	.ARM.exidx : {
		*(.ARM.exidx*)
	} SECTION_REGION(text)
	__exidx_start = ADDR(.ARM.exidx);
	__exidx_end = __exidx_start + SIZEOF(.ARM.exidx);
}
