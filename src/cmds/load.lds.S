
#include <asm-generic/embox.lds.h>

SECTIONS {
	.rodata : {
		ALIGNMENT(); _load_image_start = .;
		*(.load_image)
		ALIGNMENT(); _load_image_end = .;
	} SECTION_REGION(rodata)
}

