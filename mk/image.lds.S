
#include <asm-generic/embox.lds.h>

SECTIONS {
	.text : {
		REGION_ALIGN();

		*(.text)
		*(.text.*)

	} SECTION_REGION(text)
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

	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)

	.data : {
		REGION_ALIGN();

		*(.data)
		*(.sdata)
		*(.data.*)
		*(.got)
		*(.got.*)

	} SECTION_REGION(data)
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
	} SECTION_REGION(bss)
	SECTION_SYMBOLS(bss)

	_bss_len = _bss_end - _bss_vma;
	_bss_len_with_reserve = SIZEOF(.bss);

	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
		*(.note.*)
	}
}
