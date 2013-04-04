
#include <asm-generic/embox.lds.h>

SECTIONS {
	.free_sect (NOLOAD) : {
		. = ABSOLUTE(_mem_begin + _mem_length);
		_free_end = .;
	} > sdram AT> sdram
}
