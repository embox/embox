#include <asm-generic/embox.lds.h>

SECTIONS {
	.rodata () : {
		ALIGNMENT();
		_initfs_start = .;
		*(.initfs.rodata)
		_initfs_end = .;
	} SECTION_REGION(rodata)
}
