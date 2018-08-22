
#include <asm-generic/embox.lds.h>
#include <asm-generic/static_irq.h>

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(start)

MEMORY {
	MEMORY_REGION(RAM)
	MEMORY_REGION(ROM)
#if HAS_MEMORY_REGION(SRAM_CCM)
	MEMORY_REGION(SRAM_CCM)
#endif
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);


SECTIONS {
	.text : {
		*(.trap_table)
		*(.monitor_trap_table)
		STATIC_IRQ_TABLE

		*(.flash)

		*(.text)
		*(.text.*)
		*(.glue_7t) *(.glue_7)
	}

	/* XXX symbols __exidx_start/end are required by libgcc.a --Alexander */
	.ARM.exidx : {
		*(.ARM.exidx*)
	} SECTION_REGION(text)
	__exidx_start = ADDR(.ARM.exidx);
	__exidx_end = __exidx_start + SIZEOF(.ARM.exidx);
}
