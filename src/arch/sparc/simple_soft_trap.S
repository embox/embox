	.section    ".text"
	.align  4

	.global test_soft_traps_func
test_soft_traps_func:
	retl
	add		%o0, 1, %o0
