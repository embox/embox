#include <asm-generic/embox.lds.h>
#include <module/embox/mem/static_heap.h>

SECTIONS {
	.reserve.heap (NOLOAD): {
		REGION_ALIGN();
		_heap_start = .;
		. += OPTION_MODULE_GET(embox__mem__static_heap,NUMBER,heap_size);
		_heap_end = .;
	}
}
