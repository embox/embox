#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

SECTIONS {
	.bss..reserve.stack (NOLOAD) : ALIGN(OPTION_GET(NUMBER, alignment)) {
		. += OPTION_GET(NUMBER, stack_size);
		_stack_top = .;
	}
}
