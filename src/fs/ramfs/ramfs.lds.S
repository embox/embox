#include <asm-generic/embox.lds.h>
SECTIONS {
	.rodata (NOLOAD) : {
		ALIGNMENT();
		_ramfs_start = .;
		*(.ramfs)
		_ramfs_end = .;
		ALIGNMENT();
	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)
}
