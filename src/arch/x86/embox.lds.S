
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
		REGION_ALIGN();
		*(.multiboot)
		_traps_text_start = .;
		*(.traps.*)
		_traps_text_end = .;
		*(.text)
		*(.text.*)
		REGION_ALIGN();
	} SECTION_REGION(text) :text
	SECTION_SYMBOLS(text)

	.rodata : {
		REGION_ALIGN();
		LDS_INPUT_RODATA
		REGION_ALIGN();
	} SECTION_REGION(rodata) :text
	SECTION_SYMBOLS(rodata)

	.data : {
		REGION_ALIGN();
		LDS_INPUT_DATA
		*(.got)
		*(.got.*)
		REGION_ALIGN();
	} SECTION_REGION(data) :data
	SECTION_SYMBOLS(data)

	.bss : {
		REGION_ALIGN();
		LDS_INPUT_BSS
		REGION_ALIGN();
	} SECTION_REGION(bss) :data
	SECTION_SYMBOLS(bss)

	.reserve (NOLOAD) : {
		REGION_ALIGN();
		LDS_INPUT_RESERVE
		REGION_ALIGN();
		/* TODO memory region for reserve output section. */
	} SECTION_REGION(bss) :data
	SECTION_SYMBOLS(reserve)

	/* Sections to be discarded */
	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
		*(.note.*)
	}
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;
