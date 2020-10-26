#include <asm-generic/embox.lds.h>

#include <section_symbols.lds.h>

#include <framework/mod/options.h>

SECTIONS {
	.data.retained : {
		. = ALIGN(4);
		__RetRAM0_start = .;
		*(nmi_info)
		*(hard_fault_info)
		*(retention_mem_uninit)
		. = ALIGN(32);

		__RetRAM0_data_start = .;
		*(privileged_data_zi)
		*(retention_mem_zi)

		. = ALIGN(32);
		__RetRAM0_data_end__ = .;
	}
}
