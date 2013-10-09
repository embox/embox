
#include <asm-generic/embox.lds.h>
PHDRS {
	text     PT_LOAD ;
	data     PT_LOAD ;
}

SECTIONS {
	.text : {
		REGION_ALIGN();

		*(.text)
		*(.text.*)

	} SECTION_REGION(text) :text
	SECTION_SYMBOLS(text)

	.rodata : {
		REGION_ALIGN();

		*(.rodata)
		*(.rodata.*)
		*(.const)

		_ctors_start = .;
		KEEP(*(.init_array))
		KEEP(*(.ctors))
		KEEP(*(.ctors.*))
		_ctors_end   = .;

		/*_eh_frame_start = .;*/
		__EH_FRAME_BEGIN__ = .;
		KEEP(*(.eh_frame))
		/*KEEP(*(.eh_frame.*))*/
		/*KEEP(*(.gcc_except_table))*/

		ALIGNMENT();
		*(.checksum)

	} SECTION_REGION(rodata) :text
	SECTION_SYMBOLS(rodata)

	.data : {
		REGION_ALIGN();

		*(.data)
		*(.sdata)
		*(.data.*)
		*(.got)
		*(.got.*)

	} SECTION_REGION(data) :data
	SECTION_SYMBOLS(data)

	.bss : {
		REGION_ALIGN();

		*(.bss)
		*(.sbss)
		*(.bss.*)

		ALIGNMENT();
		_bss_end = .;
		/* Reserve (no need to zero on startup) */

		*(.reserve)
		*(.reserve.*)

		ALIGNMENT();
		_reserve_end = .;
	} SECTION_REGION(bss) :data
	SECTION_SYMBOLS(bss)

	_bss_len = _bss_end - _bss_vma;
	_bss_len_with_reserve = SIZEOF(.bss);

	/DISCARD/ : {
		*(.comment)
		*(.note.*)
	}
}
