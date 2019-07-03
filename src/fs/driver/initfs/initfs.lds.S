SECTIONS {
	.rodata.embox.initfs : {
		_initfs_start = .;
		*(.rodata.embox.initfs)
		_initfs_end = .;
	}
}
