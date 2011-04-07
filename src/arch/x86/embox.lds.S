
#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start)

MEMORY {
	MEMORY_REGION(sdram)
	MEMORY_REGION(flash)
}


_text_start = _text_vma;
_text_end = _text_vma + _text_len;
SECTIONS {
	.text : {
		ALIGNMENT();
		*(.multiboot)
		*(.text)
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
		_data_start = .;
		*(.data)
		ALIGNMENT();
		_data_end = .;
	} SECTION_REGION(data)
	SECTION_SYMBOLS(data)

	.bss : {
		ALIGNMENT();
		_bstart = .;
		*(.bss)
		ALIGNMENT();
		_bend = .;
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(bss)

	.stack (NOLOAD): {
		ALIGNMENT();
		_stack_bottom = .;
		. += CONFIG_STACK_SIZE;
		ALIGNMENT();
		_stack_top = .;
		_stack = _stack_top;
		__stack = _stack;
	} SECTION_REGION(stack)

	.plt 0 : {
		*(.plt) *(.iplt)
	}
	.got 0 : {
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

	/*
	/DISCARD/ : {
		*(.note)
		*(.note.ABI-tag)
		*(.eh_frame)
		*(.comment)
	}
	*/
}

