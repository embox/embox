
SECTIONS {
	.flash (NOLOAD): ALIGN(0x0000800) {
		_flash_start = .;
		. += 0x000010000;
		_flash_end = .;
	}
}
