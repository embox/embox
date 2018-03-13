#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf64-e2k", "elf64-e2k", "elf64-e2k")
OUTPUT_ARCH(e3s)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(RAM)
	MEMORY_REGION(ROM)
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		_start = .;
		/* 0x00FF0000 */
		*(.x86_boot)

		. = ALIGN(0x10000);
		*(.e2k_entry)
	}
}
