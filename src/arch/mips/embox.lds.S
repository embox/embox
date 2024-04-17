
#include <asm-generic/embox.lds.h>

OUTPUT_ARCH(mips)
ENTRY(_start)

MEMORY {
	#include <regions.lds.h>
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		KEEP(*(.reset))
		. = + 0x400; /* reserve for exception table */
		ALIGNMENT();
		*(.text)
		*(.text.*)
	} SECTION_REGION(text) :text
}
