
#include <asm-generic/embox.lds.h>

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
		trap_table_start = .;
		KEEP(*(.trap_table));
		trap_table_end = .;

		*(.monitor_trap_table)

		__static_irq_table_start = .;
		KEEP(*(.static_irq_table));
		__static_irq_table_end = .;

		*(.text.bootcode*)

		KEEP(*(.da14680_quirk));
		KEEP(*(.da1469x_quirk));

		KEEP(*(.inchip_flash_section));

		*(.text)
		*(.text.*)
		*(.glue_7t) *(.glue_7)
	} SECTION_REGION(text) :text

	/* XXX symbols __exidx_start/end are required by libgcc.a --Alexander */
	.ARM.exidx : {
		__exidx_start = .;
		KEEP(*(.ARM.exidx*));
		__exidx_end = .;
	} SECTION_REGION(text) :text

	.ARM.extab : {
		KEEP(*(.ARM.extab*));
	} SECTION_REGION(text) :text

}
