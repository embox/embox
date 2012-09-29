#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>
#include <module/embox/mem/page_api.h>
#include <module/embox/mem/heap_api.h>

SECTIONS {
	.heap (NOLOAD): {
		. = ALIGN(OPTION_MODULE_GET(embox__mem__page_api,NUMBER,page_size));
		_heap_start = .;
		. += OPTION_MODULE_GET(embox__mem__heap_api,NUMBER,heap_size);
		. = ALIGN(OPTION_MODULE_GET(embox__mem__page_api,NUMBER,page_size));
		_heap_end = .;
	} SECTION_REGION(heap)
	SECTION_SYMBOLS(heap)
}
