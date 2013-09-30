
#include <asm-generic/embox.lds.h>

PHDRS {
	text PT_LOAD FLAGS(5);  /* R_E */
	data PT_LOAD FLAGS(6);  /* RW_ */
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
		KEEP(*(SORT(.init_array)))
		KEEP(*(SORT(.ctors)))
		KEEP(*(SORT(.ctors.*)))
		_ctors_end   = .;

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
		*(.eh_frame)
		*(.note.*)
	}
}
