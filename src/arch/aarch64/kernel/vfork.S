/**
* @file vfork.S
* @brief vfork implementation for aarch64
* @author Denis Deryugin <deryugin.denis@gmail.com>
* @version 
* @date 07.08.2019
*/

.text
.global vfork

vfork:
	/* Fill struct pt_regs on current stack */
	sub sp, sp, #31 * 8
	stp x0, x1,   [sp]
	stp x2, x3,   [sp, #16] !
	stp x4, x5,   [sp, #16] !
	stp x6, x7,   [sp, #16] !
	stp x8, x9,   [sp, #16] !
	stp x10, x11, [sp, #16] !
	stp x12, x13, [sp, #16] !
	stp x14, x15, [sp, #16] !
	stp x16, x17, [sp, #16] !
	stp x18, x19, [sp, #16] !
	stp x20, x21, [sp, #16] !
	stp x22, x23, [sp, #16] !
	stp x24, x25, [sp, #16] !
	stp x26, x27, [sp, #16] !
	stp x28, x29, [sp, #16] !
	add x2, sp, 0x18
	stp x30, x2, [sp, #16] ! 
	/* x0 should point to the structure accroding to ABI */
	mov x0, sp
	sub x0, x0, #30 * 8
	sub sp, x0, #8
	b vfork_body
