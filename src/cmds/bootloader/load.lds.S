
#include <asm-generic/embox.lds.h>

SECTIONS {
	.rodata.load : {
		ALIGNMENT(); _load_image_start = .;
		*(.load_image)
		ALIGNMENT(); _load_image_end = .;
	}
}
