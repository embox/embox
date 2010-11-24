
#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start)

_text_start = _text_vma;
_text_end = _text_vma + _text_len;
SECTIONS {
	. = 0x100000;
	.text : {
		*(.text)
		*(.init.text)
	}
	SECTION_SYMBOLS(text)

	.rodata : {
		LDS_INPUT_RODATA
	}
	SECTION_SYMBOLS(rodata)

	.data : {
		_data_start = .;
		*(.data)
		_data_end = .;
	}
	SECTION_SYMBOLS(data)

	.bss : {
		_bstart = .;
		*(.bss)
		_bend = .;
	}
	SECTION_SYMBOLS(bss)
}

