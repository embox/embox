
SECTIONS {
	.flash (NOLOAD): ALIGN(0x4000) {
		_flash_start = .;
		. += 0x0000c000;
		_flash_end = .;
	}
}

