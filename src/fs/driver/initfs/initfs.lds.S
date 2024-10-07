#include <framework/mod/options.h>

#define INITFS_SECTION OPTION_GET(STRING,section)

SECTIONS {
	INITFS_SECTION : {
		_initfs_start = .;
		*(.rodata.embox.initfs_image)
		_initfs_end = .;
	}
}
