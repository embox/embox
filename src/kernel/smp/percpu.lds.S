#include <asm-generic/embox.lds.h>
#include <framework/mod/options.h>

#include <kernel/cpu.h>

SECTIONS {
	.data : {
		_percpu_vma = .;
		*(.data.percpu)
		_percpu_len = . - _percpu_vma;
		. += _percpu_len * (NCPU - 1);
	} SECTION_REGION(data) :data
	SECTION_SYMBOLS(data)
}
