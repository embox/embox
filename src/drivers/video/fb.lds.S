#include <asm-generic/embox.lds.h>

#include <framework/mod/options.h>

SECTIONS {
	.framebuffer (NOLOAD): ALIGN(DEFAULT_DATA_ALIGNMENT) {
		__framebuffer_start = .;
		. += OPTION_GET(NUMBER,size);
		__framebuffer_end = .;
	}
}
