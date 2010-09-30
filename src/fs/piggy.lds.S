	.rodata (NOLOAD) : {
		ALIGNMENT();
		_piggy_start = .;
		PIGGY
		_piggy_end = .;
		ALIGNMENT();
	} SECTION_REGION(rodata)
	SECTION_SYMBOLS(rodata)
