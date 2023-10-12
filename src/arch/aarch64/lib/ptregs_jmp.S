/**
* @file ptregs_jmp.S
* @brief ptregs_jmp implementation for aarch64
* @author Denis Deryugin <deryugin.denis@gmail.com>
* @version 
* @date 07.08.2019
*/

.text
.global ptregs_jmp

ptregs_jmp:
	add     x0, x0, #29 * 8
	ldp	x29, x30, [x0]
	add	sp, x29, 0
	sub     x0, x0, #29 * 8
	mov	x29, x0
	ldp     x0, x1, [x29]
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

	ret
