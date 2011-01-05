#include <asm-generic/embox.lds.h>
SECTIONS {
	.rodata : {
		ALIGNMENT();
		_drivers_fs_start = .;
		*(.drivers.fs)
		_drivers_fs_end = .;
		ALIGNMENT();
	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)
}
