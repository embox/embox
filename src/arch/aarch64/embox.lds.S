#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(aarch64_reset_handler)

MEMORY {
	#include <regions.lds.h>
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		KEEP(*(.bootcode));

		*(.text)
		*(.text.*)
		*(.glue_7t) *(.glue_7)
	} SECTION_REGION(text) :text
}
