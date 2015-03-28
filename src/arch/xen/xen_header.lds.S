
PHDRS {
	xen 	 PT_NOTE;
}

SECTIONS {
	.note : {
		*(.note)
	} :xen
}

