/**
 * @file
 * @brief TODO
 *
 * @date Mar 1, 2013
 * @author Eldar Abusalimov
 */
	.text
	.align 4

	.global __ashldi3
__ashldi3:
	tst     %o2
	be      3f
	 mov    32, %g2

	sub     %g2, %o2, %g2

	tst     %g2
	bg      1f
	 srl    %o1, %g2, %g3

1:
	clr     %o5
	neg     %g2
	ba      2f
	 sll    %o1, %g2, %o4


	sll     %o1, %o2, %o5
	srl     %o0, %o2, %g2
	or      %g2, %g3, %o4
2:
	mov     %o4, %o0
	mov     %o5, %o1
3:
	jmpl    %o7 + 8, %g0
	 nop
