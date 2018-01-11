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
	. = 0x1000000 - 0x10000; /* Fixed entry address */
	.data : {
		*(.x86_boot)
	}

	. = 0x1000000;
	_start = .;
	.text : {
		. = 0x1006000;
		*(.e2k_entry)
	}
}
