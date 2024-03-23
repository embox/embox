
#include <asm-generic/embox.lds.h>

#include <framework/mod/options.h>
#include <module/embox/mem/page_api.h>

/* needed for vmem */
#define ARCH_TEXT_ALIGNMENT PAGE_SIZE()
#define ARCH_DATA_ALIGNMENT PAGE_SIZE()

OUTPUT_FORMAT("elf32-microblaze", "elf32-microblaze", "elf32-microblaze-little")
OUTPUT_ARCH(microblaze)
ENTRY(_start)

MEMORY {
	#include <regions.lds.h>
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		*(.trap_table.text)
		*(.text)
		*(.text.*)
	} SECTION_REGION(text) :text

	_bstart = _bss_vma;
	_bend = _bstart + _bss_len;

}

