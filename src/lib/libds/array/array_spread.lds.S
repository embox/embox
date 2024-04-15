
SECTIONS {
	.rodata : {
		*(SORT(.array_spread.*.rodata))
	}
}
