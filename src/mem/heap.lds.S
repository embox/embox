#include <asm-generic/embox.lds.h>

#include <framework/mod/options.h>
#include <module/embox/mem/static_heap.h>

#define _HEAP_SECTION OPTION_MODULE_GET(embox__mem__static_heap,STRING,section)

SECTIONS {
	_HEAP_SECTION.heap (NOLOAD): ALIGN(DEFAULT_DATA_ALIGNMENT) {
		_heap_start = .;
		. += OPTION_MODULE_GET(embox__mem__static_heap,NUMBER,heap_size);
		_heap_end = .;
	}
}
