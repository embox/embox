#include <asm-generic/embox.lds.h>

#include <framework/mod/options.h>
#include <module/embox/mem/static_heap.h>

SECTIONS {
	.bss..reserve.heap (NOLOAD): ALIGN(DEFAULT_DATA_ALIGNMENT) {
		_heap_start = .;
		. += OPTION_MODULE_GET(embox__mem__static_heap,NUMBER,heap_size);
		_heap_end = .;
	}
}
