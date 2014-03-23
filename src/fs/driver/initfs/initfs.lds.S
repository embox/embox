SECTIONS {
	.rodata.initfs : {
		_initfs_start = .;
		*(.rodata.initfs)
		_initfs_end = .;
	}
}
