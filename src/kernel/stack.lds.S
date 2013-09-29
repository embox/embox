#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

#define STACK_ALIGNMENT() \
	. = ALIGN(OPTION_GET(NUMBER, alignment))

SECTIONS {
	.reserve.stack (NOLOAD) : {
		STACK_ALIGNMENT();
		. += OPTION_GET(NUMBER, stack_size);
		STACK_ALIGNMENT();
		__stack = .;
	}
}
