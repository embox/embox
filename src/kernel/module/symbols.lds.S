#include <asm-generic/embox.lds.h>

SECTIONS {
	.symtab : {
		_symtab_start = .;
		*(.symtab*)
		_symtab_end = .;
		_strtab_start = .;
        *(.strtab*)
	} > sdram
}
