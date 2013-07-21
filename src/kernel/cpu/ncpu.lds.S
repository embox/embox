#include <asm-generic/embox.lds.h>
#include <kernel/cpu.h>

SECTIONS {
	.data : {
		__cpudata_start = .;
		*(.cpudata.data)
		__cpudata_block_end = .;
		. += (__cpudata_block_end - __cpudata_start) * (NCPU - 1);
		__cpudata_end = .;
	} SECTION_REGION(data) :data
	SECTION_SYMBOLS(data)
}
