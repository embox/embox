#include <asm-generic/embox.lds.h>


SECTIONS {
	/DISCARD/ : {
		*(.note)
		*(.note.ABI-tag)
		*(.eh_frame) /* TODO this section required if c++ included */
		*(.comment)
	}

	.global_ctors : {
		ALIGNMENT();
        _ctors_start = .;
		KEEP(*(SORT(.init)))
		KEEP(*(SORT(.init.*)))
		KEEP(*(SORT(.ctors)))
		KEEP(*(SORT(.ctors.*)))
        _ctors_end   = .;
        ALIGNMENT();
    } SECTION_REGION(rodata)
	SECTION_SYMBOLS(global_ctors)

}
