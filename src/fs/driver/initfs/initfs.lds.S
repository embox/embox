#include <asm-generic/embox.lds.h>

SECTIONS {
	.rodata () : {
		REGION_ALIGN();
		_initfs_start = .;
		*(.initfs.rodata)
		_initfs_end = .;
		REGION_ALIGN();
	} SECTION_REGION(rodata)
}
