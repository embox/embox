
#include <asm-generic/embox.lds.h>

#include <framework/mod/options.h>
#include <module/embox/mem/page_api.h>

#define ARCH_TEXT_ALIGNMENT PAGE_SIZE()
#define ARCH_DATA_ALIGNMENT PAGE_SIZE()

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

MEMORY {
	#include <regions.lds.h>
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		*(.multiboot)
		_traps_text_start = .;
		*(.traps.*)
		_traps_text_end = .;
		*(.text)
		*(.text.*)
	}
#if 0
	.eh_frame : {
		*(.eh_frame)
	} SECTION_REGION(data)
#endif
}
