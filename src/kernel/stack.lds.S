#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

#define STACK_ALIGNMENT OPTION_GET(NUMBER,alignment)

SECTIONS {
	.stack BLOCK(STACK_ALIGNMENT) (NOLOAD) : {
		. = ALIGN (STACK_ALIGNMENT);
		ALIGNMENT();
		. += OPTION_GET(NUMBER,stack_size);
		ALIGNMENT();
		. = ALIGN(STACK_ALIGNMENT);
		__stack = .;
	} SECTION_REGION(stack)
	SECTION_SYMBOLS(stack)
}
