
#include <asm-generic/embox.lds.h>

SECTIONS {
	.free_sect (NOLOAD) : {
		. = ABSOLUTE(_ram_base + _ram_size);
		_free_end = .;
	} > sdram AT> sdram
}
