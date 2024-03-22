
#include <asm-generic/embox.lds.h>

#ifndef OUTPUT_FORMAT_STRING
#define OUTPUT_FORMAT_STRING "elf32-bigmips"
#endif

OUTPUT_FORMAT(OUTPUT_FORMAT_STRING)
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

