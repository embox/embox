#include <asm-generic/embox.lds.h>

#include <framework/mod/options.h>

SECTIONS {
	.data.cmi_fw : ALIGN(0x400) {
		__cmi_section_start__ = .;
		. = ALIGN(0x400); /* Code region should start at 1Kb boundary */
		cmi_fw_dst_addr = .;
		KEEP(*(.cmi_fw_area*))
		. = ALIGN(0x400);
		__cmi_section_end__ = . - 1;
	}

	.data.retained : {
		__copy_table_start__ = .;
		*(text_retained)
		*(nmi_info)
		__copy_table_end__ = .;
		*(retention_mem_const)
		*(retention_mem_init)
	}
	.bss.retained (NOLOAD) : {
		__zero_table_start__ = .;
		*(retention_mem_zi);
		__zero_table_end__ = .;
	}
}
