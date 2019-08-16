#include <asm-generic/embox.lds.h>
#include <asm-generic/static_irq.h>

OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(aarch64_exception_table)

MEMORY {
	#include <regions.lds.h>
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		*(.trap_table)
		STATIC_IRQ_TABLE

		*(.flash)

		*(.text)
		*(.text.*)
		*(.glue_7t) *(.glue_7)
	}
}
