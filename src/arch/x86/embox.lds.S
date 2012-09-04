
#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}

SECTIONS {
	.text : {
		ALIGNMENT();
		. = ALIGN(0x100);
		*(.multiboot)
		*(.text)
		*(.text.*)
		*(.init.text)
		ALIGNMENT();
		. = ALIGN(0x100);
	} SECTION_REGION(text)
	SECTION_SYMBOLS(text)

	.rodata : {
		LDS_INPUT_RODATA
	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)

	.data : {
		ALIGNMENT();
		*(.data)
		ALIGNMENT();
	} SECTION_REGION(data)
	SECTION_SYMBOLS(data)

	.bss : {
		ALIGNMENT();
		*(.bss)
		ALIGNMENT();
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(bss)

	.reserve (NOLOAD): {
		LDS_INPUT_RESERVE
	/* TODO memory region for reserve output section. */
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(reserve)

	.stack (NOLOAD): {
		ALIGNMENT();
		. += OPTION_GET(NUMBER,stack_size);
		ALIGNMENT();
	} SECTION_REGION(stack)
	SECTION_SYMBOLS(stack)

#if 0
/* this is required for dynamic linkage library */
	.plt 0 : { /* procedure linkage table */
		*(.plt) *(.iplt)
	}
	.got 0 : { /* global offset table */
		*(.got) *(.igot)
	}
	.got.plt 0 : {
		*(.got.plt)  *(.igot.plt)
	}
	.rel.dyn 0 : {
		*(.rel.init)
		*(.rel.text .rel.text.* .rel.gnu.linkonce.t.*)
		*(.rel.fini)
		*(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*)
		*(.rel.data.rel.ro* .rel.gnu.linkonce.d.rel.ro.*)
		*(.rel.data .rel.data.* .rel.gnu.linkonce.d.*)
		*(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*)
		*(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*)
		*(.rel.ctors)
		*(.rel.dtors)
		*(.rel.got)
		*(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*)
		*(.rel.ifunc)
	}
	.rel.plt 0 : {
		*(.rel.plt)
		*(.rel.iplt)
	}
#endif

	/DISCARD/ : {
		*(.note)
		*(.note.ABI-tag)
		*(.eh_frame)
		*(.comment)
	}
}

_mem_begin = LDS_REGION_BASE_sdram;
_mem_length = LDS_REGION_SIZE_sdram;
