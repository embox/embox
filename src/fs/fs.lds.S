#include <asm-generic/embox.lds.h>
SECTIONS {
	.rodata (NOLOAD) : {
		ALIGNMENT();
		_initfs_start = .;
		*(.drivers.fs)
		_initfs_end = .;
		ALIGNMENT();
	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)
}
