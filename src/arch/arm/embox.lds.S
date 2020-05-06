
#include <asm-generic/embox.lds.h>
#include <asm-generic/static_irq.h>

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(start)

MEMORY {
	#include <regions.lds.h>
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		*(.trap_table)
		*(.monitor_trap_table)
		STATIC_IRQ_TABLE

		*(.da14680_quirk)
		*(.da1469x_quirk)

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
