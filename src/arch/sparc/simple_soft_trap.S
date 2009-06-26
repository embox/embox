
	.section    ".text"
	.align  4

	.global test_soft_traps_func
test_soft_traps_func:
 	set		test_soft_traps_variable, %l4
 	ld 		[%l4], %l5
 	add		%l5, 1,%l5
	st 		%l5, [%l4]

	jmp    %l2
	 rett    %l2 + 4
	nop
	nop
