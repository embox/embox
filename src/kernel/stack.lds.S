#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

SECTIONS {
	.stack (NOLOAD): {
		ALIGNMENT();
		. += OPTION_GET(NUMBER,stack_size);
		ALIGNMENT();
		__stack = .;
	} SECTION_REGION(stack)
	SECTION_SYMBOLS(stack)
}
