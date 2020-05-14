#include <asm-generic/embox.lds.h>

#include <framework/mod/options.h>

SECTIONS {
	.data.retained : {
		__copy_table_start__ = .;
		*(text_retained)
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
