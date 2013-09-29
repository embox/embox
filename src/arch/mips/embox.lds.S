
#include <asm-generic/embox.lds.h>

#ifndef OUTPUT_FORMAT_STRING
#define OUTPUT_FORMAT_STRING "elf32-bigmips"
#endif

OUTPUT_FORMAT(OUTPUT_FORMAT_STRING)
OUTPUT_ARCH(mips)

ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;

SECTIONS {
	.text : {
		KEEP(*(.reset))
		. = + 0x400; /* reserve for exception table */
		ALIGNMENT();
		*(.text)
		*(.text.*)
	}

	/DISCARD/ : {
		*(.MIPS.options)
		*(.options)
		*(.pdr)
		*(.reginfo)
		*(.comment)
		*(.note)
		*(.mdebug.* .gnu.attributes)
	}
}

