
#include <asm-generic/embox.lds.h>
#include <apps.lds.h>

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
	#include <phdrs.lds.h>
}

SECTION_SYMBOLS(text)
SECTION_SYMBOLS(rodata)
SECTION_SYMBOLS(data)
SECTION_SYMBOLS(bss)
#include <section_symbols.lds.h>

SECTIONS {
	.text : ALIGN(TEXT_ALIGNMENT) {

		LDS_MODULES_TEXT
		*(.text)
		*(.text.*)

	} SECTION_REGION(text) :text

	.rodata : ALIGN(DATA_ALIGNMENT) {

		LDS_MODULES_RODATA
		*(.rodata)
		*(.rodata.*)
		*(.srodata)
		*(.srodata.*)
		*(.const)

		ALIGNMENT();
		_ctors_start = .;
		KEEP(*(.init_array))
		KEEP(*(.init_array.*))
		KEEP(*(.ctors))
		KEEP(*(.ctors.*))
		_ctors_end   = .;

		ALIGNMENT();
		_dtors_start = .;
		KEEP(*(.fini_array))
		KEEP(*(.fini_array.*))
		KEEP(*(.dtors))
		KEEP(*(.dtors.*))
		_dtors_end   = .;

		ALIGNMENT();
		_eh_frame_begin = .;
		KEEP(*(.eh_frame))
		KEEP(*(.eh_frame.*))
		KEEP(*(.eh_frame_end))
		/* KEEP(*(.gcc_except_table*)) */

		ALIGNMENT();
		*(.checksum)

	} SECTION_REGION(rodata) :rodata

	.data : ALIGN(DATA_ALIGNMENT) {

		LDS_MODULES_DATA
		_app_data_vma = .;
		LDS_APPS_DATA
		*(.data)
		*(.data.*)
		*(.sdata)
		*(.sdata.*)
		*(.got)
		*(.got.*)

	} SECTION_REGION(data) :data
	LDS_MODULES_DATA_LMA

	.bss : ALIGN(DATA_ALIGNMENT) {

		LDS_MODULES_BSS
		LDS_APPS_BSS
		*(.bss)
		*(.sbss)
		*(.sbss.*)
		*(.bss.[a-zA-Z0-9_-]*) /* this hell is to exclude '.bss..reserve'. */
		/* *(.tbss) */
		/* *(.tbss.*) */

		ALIGNMENT();
		_bss_end = .;
		/* Reserve (no need to zero on startup) */

		*(.bss..reserve)
		*(.bss..reserve.*)

		LDS_APP_DATA_RESERVE_BSS

		ALIGNMENT();
		_reserve_end = .;
	} SECTION_REGION(bss) :bss

	_bss_len = _bss_end - _bss_vma;
	_bss_len_with_reserve = SIZEOF(.bss);

	#include <sections.lds.h>

	/DISCARD/ : {
		*(.comment)
		*(.note.*)
	}
}
