
SECTIONS {
	.rodata.initfs : {
		_initfs_start = .;
		*(.initfs.rodata)
		_initfs_end = .;
	}
}
