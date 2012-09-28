#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

SECTIONS {
	.stack (NOLOAD): {
		. = ALIGN(0x1000);
		ALIGNMENT();
		. += OPTION_GET(NUMBER,stack_size);
		ALIGNMENT();
		__stack = .;
	} SECTION_REGION(stack)
	SECTION_SYMBOLS(stack)
}
