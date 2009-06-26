	.section    ".text"
	.align  4

	.global test_soft_traps_func
test_soft_traps_func:
	add		%o0, 1, %o0

	jmp		%l1	! old PC
	rett	%l2	! old nPC
	nop
	nop
