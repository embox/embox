
#include <asm-generic/embox.lds.h>

#include <framework/mod/options.h>
#include <module/embox/mem/page_api.h>

#define ARCH_TEXT_ALIGNMENT PAGE_SIZE()
#define ARCH_DATA_ALIGNMENT PAGE_SIZE()

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(RAM)
	MEMORY_REGION(ROM)
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		. = ALIGN(0x1000);
		hypercall_page = .;
		. += 0x1000;

		xen_shared_info = .;
		. += 0x1000;
		xen_start_info = .;
		. += 0x1000;

		_traps_text_start = .;
		*(.traps.*)
		_traps_text_end = .;
		*(.text)
		*(.text.*)
	} SECTION_REGION(text) :text
#if 0
	.eh_frame : {
		*(.eh_frame)
	} SECTION_REGION(data)
#endif
}
