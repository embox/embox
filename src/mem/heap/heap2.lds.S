#include <asm-generic/embox.lds.h>

#include <framework/mod/options.h>

#define _HEAP_SECTION OPTION_GET(STRING,section)

#define HEAP_START OPTION_GET(STRING,start_symbol)
#define HEAP_END OPTION_GET(STRING,end_symbol)

SECTIONS {
	_HEAP_SECTION.heap (NOLOAD): ALIGN(DEFAULT_DATA_ALIGNMENT) {
		HEAP_START = .;
		. += OPTION_GET(NUMBER,heap_size);
		HEAP_END = .;
	}
}
