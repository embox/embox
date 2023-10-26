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
	/* Save current context (struct context) */
	mov     x2, sp
	mrs     x3, spsr_el1
	mrs     x4, daif
	stp     x19, x20, [x0]
	stp     x21, x22, [x0, #16]!
	stp     x23, x24, [x0, #16]!
	stp     x25, x26, [x0, #16]!
	stp     x27, x28, [x0, #16]!
	stp     x29, x30, [x0, #16]!
	stp     x2, x3, [x0, #16]!
	str     x4, [x0, #16]

	/* Load previous context (struct context) */
	ldp     x19, x20, [x1]
	ldp     x21, x22, [x1, #16]!
	ldp     x23, x24, [x1, #16]!
	ldp     x25, x26, [x1, #16]!
	ldp     x27, x28, [x1, #16]!
	ldp     x29, x30, [x1, #16]!
	ldp     x2, x3, [x1, #16]!
	ldr     x4, [x1, #16]
	mov     sp, x2
	// msr spsr_el1, x3
	msr     daif, x4

	ret     /* Jump to x30 */
