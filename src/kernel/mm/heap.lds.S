#include <asm-generic/embox.lds.h>

SECTIONS {
	.heap (NOLOAD): {
		. = ALIGN(CONFIG_PAGE_SIZE);
		_heap_start = .;
		. += CONFIG_HEAP_SIZE;
		. = ALIGN(CONFIG_PAGE_SIZE);
		_heap_end = .;
	} SECTION_REGION(heap)
	SECTION_SYMBOLS(heap)
}
