#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(aarch64_trap_table)

MEMORY {
	#include <regions.lds.h>
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		KEEP(*(.trap_table));

		*(.text)
		*(.text.*)
		*(.glue_7t) *(.glue_7)
	} > RAM AT> RAM :text
}
