#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

SECTIONS {
	.bss.thread_heap (NOLOAD): ALIGN(DEFAULT_DATA_ALIGNMENT) {
		thread_heap_start = .;
		. += OPTION_GET(NUMBER,thread_heap_size);
	}
}
