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

	/* XXX symbols __exidx_start/end are required by libgcc.a --Alexander */
	.ARM.exidx : {
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} SECTION_REGION(text)

	.ARM.extab : {
		*(.ARM.extab*)
	} SECTION_REGION(text)
}
