#include <asm-generic/embox.lds.h>

SECTIONS {
	.rodata.initfs : {
		_initfs_start = .;
		*(.rodata.initfs)
		_initfs_end = .;
	} SECTION_REGION(rodata) :rodata
}
