#include <asm-generic/embox.lds.h>


SECTIONS {
	/DISCARD/ : {
		*(.note)
		*(.note.ABI-tag)
		*(.eh_frame)
		*(.comment)
	}
}
