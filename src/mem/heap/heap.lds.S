#include <asm-generic/embox.lds.h>

#include <framework/mod/options.h>

#define _HEAP_SECTION OPTION_GET(STRING,section)

SECTIONS {
	_HEAP_SECTION.heap (NOLOAD): ALIGN(DEFAULT_DATA_ALIGNMENT) {
		_heap_start = .;
		. += OPTION_GET(NUMBER,heap_size);
		_heap_end = .;
	}
}
