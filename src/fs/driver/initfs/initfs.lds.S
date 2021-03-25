#include <framework/mod/options.h>

#define INITFS_SECTION OPTION_GET(STRING,section)

SECTIONS {
	INITFS_SECTION.initfs : {
		_initfs_start = .;
		*(.rodata.embox.initfs)
		_initfs_end = .;
	}
}
