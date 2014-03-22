#include <asm-generic/embox.lds.h>

SECTIONS {
	.rodata : {
		*(SORT(.array_spread.*.rodata))
	} SECTION_REGION(rodata) :rodata
}
