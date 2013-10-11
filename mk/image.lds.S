
#include <asm-generic/embox.lds.h>

#ifdef ARCH_TEXT_ALIGNMENT
# define TEXT_ALIGNMENT    ARCH_TEXT_ALIGNMENT
#else
# define TEXT_ALIGNMENT    DEFAULT_TEXT_ALIGNMENT
#endif

#ifdef ARCH_DATA_ALIGNMENT
# define DATA_ALIGNMENT    ARCH_DATA_ALIGNMENT
#else
# define DATA_ALIGNMENT    DEFAULT_DATA_ALIGNMENT
#endif

PHDRS {
	text     PT_LOAD  FLAGS(5);  /* R_E */
	rodata   PT_LOAD  FLAGS(5);  /* R_E */
	data     PT_LOAD  FLAGS(6);  /* RW_ */
	bss      PT_LOAD  FLAGS(6);  /* RW_ */
}

SECTION_SYMBOLS(text)
SECTION_SYMBOLS(rodata)
SECTION_SYMBOLS(data)
SECTION_SYMBOLS(bss)

SECTIONS {
	.text : ALIGN(TEXT_ALIGNMENT) {

		*(.text)
		*(.text.*)

	} SECTION_REGION(text) :text

	.rodata : ALIGN(DATA_ALIGNMENT) {

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

	} SECTION_REGION(rodata) :rodata

	.data : ALIGN(DATA_ALIGNMENT) {

		*(.data)
		*(.sdata)
		*(.data.*)
		*(.got)
		*(.got.*)

	} SECTION_REGION(data) :data

	.bss : ALIGN(DATA_ALIGNMENT) {

		*(.bss)
		*(.sbss)
		*(.bss.[a-zA-Z0-9-]*) /* this hell is to exclude '.bss..reserve'. */

		ALIGNMENT();
		_bss_end = .;
		/* Reserve (no need to zero on startup) */

		*(.bss..reserve)
		*(.bss..reserve.*)

		ALIGNMENT();
		_reserve_end = .;
	} SECTION_REGION(bss) :bss

	_bss_len = _bss_end - _bss_vma;
	_bss_len_with_reserve = SIZEOF(.bss);

	/DISCARD/ : {
		*(.comment)
		*(.note.*)
	}
}
