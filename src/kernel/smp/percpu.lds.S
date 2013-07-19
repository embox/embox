#include <asm-generic/embox.lds.h>
#include <kernel/cpu.h>

SECTIONS {
	.data : {
		_percpu_start = .;
		*(.percpu.data)
		_percpu_block_end = .;
		. += (_percpu_block_end - _percpu_start) * (NCPU - 1);
		_percpu_end = .;
	} SECTION_REGION(data) :data
	SECTION_SYMBOLS(data)
}
