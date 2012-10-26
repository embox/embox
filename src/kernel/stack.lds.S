#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

#define STACK_ALIGMENT OPTION_GET(NUMBER,alignment)

SECTIONS {
	.stack (NOLOAD): {
		. = ALIGN (STACK_ALIGMENT);
		ALIGNMENT();
		. += OPTION_GET(NUMBER,stack_size);
		ALIGNMENT();
		. = ALIGN(STACK_ALIGMENT);
		__stack = .;
	} SECTION_REGION(stack)
	SECTION_SYMBOLS(stack)
}
