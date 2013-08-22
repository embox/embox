/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.08.2013
 */

#include <asm/setjmp.h>

.global setjmp
setjmp:
	pop	r14

	mov 	r1, 0(r15)
	mov 	r2, 2(r15)
	mov 	r4, 4(r15)
	mov 	r5, 6(r15)
	mov 	r6, 8(r15)
	mov 	r7, 10(r15)
	mov 	r8, 12(r15)
	mov 	r9, 14(r15)
	mov 	r10, 16(r15)
	mov 	r11, 18(r15)

	mov 	r14, 20(r15)

	clr 	r15
	br	r14


.global longjmp
longjmp:
	mov 	@r15+, r1
	mov 	@r15+, r2
	mov 	@r15+, r4
	mov 	@r15+, r5
	mov 	@r15+, r6
	mov 	@r15+, r7
	mov 	@r15+, r8
	mov 	@r15+, r9
	mov 	@r15+, r10
	mov 	@r15+, r11

	mov 	@r15, r13
	mov 	r14, r15
	br	r13

