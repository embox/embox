#include <asm-generic/embox.lds.h>

#include <framework/mod/options.h>
#include <module/embox/mem/page_api.h>

#define ARCH_TEXT_ALIGNMENT PAGE_SIZE()
#define ARCH_DATA_ALIGNMENT PAGE_SIZE()

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")

#undef i386
OUTPUT_ARCH(i386)

ENTRY(_start)

MEMORY {
	#include <regions.lds.h>
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.text : {
		KEEP(*(.multiboot));
		_traps_text_start = .;
		KEEP(*(.traps.*));
		_traps_text_end = .;
		KEEP(*(.text));
		KEEP(*(.text.*));
	} SECTION_REGION(text) :text
#if 0
	.eh_frame : {
		KEEP(*(.eh_frame))
	} SECTION_REGION(data)
#endif
}
