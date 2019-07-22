/**
* @file context_switch.S
* @brief 
* @author Denis Deryugin <deryugin.denis@gmail.com>
* @version 
* @date 19.07.2019
*/

.text
.global context_switch
context_switch:
	/* Save current context */
	stp x0, x1,   [x0]
	stp x2, x3,   [x0, #16] !
	stp x4, x5,   [x0, #16] !
	stp x6, x7,   [x0, #16] !
	stp x8, x9,   [x0, #16] !
	stp x10, x11, [x0, #16] !
	stp x12, x13, [x0, #16] !
	stp x14, x15, [x0, #16] !
	stp x16, x17, [x0, #16] !
	stp x18, x19, [x0, #16] !
	stp x20, x21, [x0, #16] !
	stp x22, x23, [x0, #16] !
	stp x24, x25, [x0, #16] !
	stp x26, x27, [x0, #16] !
	stp x28, x29, [x0, #16] !
	mov x2, sp
	stp x30, x2, [x0, #16] ! 

	mrs	x2, spsr_el1
	mrs	x3, daif
	stp x2, x3, [x0, #16] !

	/* Load previous context */
	/* At first, access return address
	 * and stack pointer */
	add	x1, x1, #(32 * 8)
	sub     x1, x1, #(2 * 8)

	add     x1, x1, #(2 * 8)
	ldp	x2, x3, [x1] 
	msr	daif, x3
	sub	x1, x1, #(2 * 8)

	ldp	x30, x29, [x1] 
	add     sp, x29, #0

	sub     x29, x1, #(32 * 8)
	ldp     x0, x1, [x29, #16]!
	ldp     x2, x3, [x29, #16]!
	ldp     x4, x5, [x29, #16]!
	ldp     x6, x7, [x29, #16]!
	ldp     x8, x9, [x29, #16]!
	ldp     x10, x11, [x29, #16]!
	ldp     x12, x13, [x29, #16]!
	ldp     x14, x15, [x29, #16]!
	ldp     x16, x17, [x29, #16]!
	ldp     x18, x19, [x29, #16]!
	ldp     x20, x21, [x29, #16]!
	ldp     x22, x23, [x29, #16]!
	ldp     x24, x25, [x29, #16]!
	ldp     x26, x27, [x29, #16]!
	ldp     x28, x29, [x29, #16]

	ret /* Jump to x30 */
