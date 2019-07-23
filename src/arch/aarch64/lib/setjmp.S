/**
* @file setjmp.S
* @brief 
* @author Denis Deryugin <deryugin.denis@gmail.com>
* @version 
* @date 22.07.2019
*/

.text
.global setjmp
setjmp:
	/* Save return address and stack pointer */
	mov x2, sp
	stp x30, x2,  [x0]
	/* Save callee-saved registers according to ABI */
	stp x19, x20, [x0, #16]!
	stp x21, x22, [x0, #16]!
	stp x23, x24, [x0, #16]!
	stp x25, x26, [x0, #16]!
	stp x27, x28, [x0, #16]!
	/* Return zero */
	mov x0, #0
	ret

.global longjmp
longjmp:
	/* Load return address and stack pointer */
	ldp x30, x2, [x0]
	add sp, x2, #0

	/* Load callee-saved registers according to ABI */
	ldp x19, x20, [x0, #16]!
	ldp x21, x22, [x0, #16]!
	ldp x23, x24, [x0, #16]!
	ldp x25, x26, [x0, #16]!
	ldp x27, x28, [x0, #16]!

	/* Return second argument */
	mov x0, x1
	ret
