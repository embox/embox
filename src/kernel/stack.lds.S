#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

SECTIONS {
	.stack ALIGN (0x1000) (NOLOAD): {
		ALIGNMENT();
		. += OPTION_GET(NUMBER,stack_size);
		ALIGNMENT();
		. = ALIGN(0x1000);
		__stack = .;
	} SECTION_REGION(stack)
	SECTION_SYMBOLS(stack)
}
