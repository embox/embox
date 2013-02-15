
#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

PHDRS {
	text PT_LOAD FLAGS(5);  /* R_E */
	data PT_LOAD FLAGS(7);  /* RWE */
	//note PT_NOTE FLAGS(0);  /* ___ */
}

SECTIONS {
	.text : {
		ALIGNMENT();
		*(.multiboot)
		_traps_text_start = .;
		*(.traps.*)
		_traps_text_end = .;
		*(.text)
		*(.text.*)
		ALIGNMENT();
	} SECTION_REGION(text) :text
	SECTION_SYMBOLS(text)

	.rodata : {
		ALIGNMENT();
		LDS_INPUT_RODATA
		ALIGNMENT();
	} SECTION_REGION(rodata) :text
	SECTION_SYMBOLS(rodata)

	.data : {
		ALIGNMENT();
		*(.data)
		ALIGNMENT();
	} SECTION_REGION(data) :data
	SECTION_SYMBOLS(data)

	.bss : {
		ALIGNMENT();
		*(.bss)
		ALIGNMENT();
	} SECTION_REGION(bss) :data
	SECTION_SYMBOLS(bss)

	.reserve (NOLOAD) : {
		ALIGNMENT();
		LDS_INPUT_RESERVE
		ALIGNMENT();
	/* TODO memory region for reserve output section. */
	} SECTION_REGION(bss) :data
	SECTION_SYMBOLS(reserve)

	/* Sections to be discarded */
	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
	}
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;
