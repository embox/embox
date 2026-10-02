#include <hal/cpu.h>

SECTIONS {
	.data.embox.cpudata : {
		__cpudata_start = .;
		*(.embox.cpudata.data)
		__cpudata_block_end = .;
		. += (__cpudata_block_end - __cpudata_start) * (NCPU - 1);
		__cpudata_end = .;
	}
}
