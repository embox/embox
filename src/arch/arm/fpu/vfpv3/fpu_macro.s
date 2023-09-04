/**
 * @file
 * @brief
 *
 * @date 25.11.22
 * @author Aleksey Zhmulin
 */

#ifndef ARM_FPU_H_
#error "Do not include this file directly!"
#endif /* ARM_FPU_H_ */

/**
 * Purpose
 * : store struct fpu_context on the stack
 *
 * Params
 * : tmp = temporary register
 * : stack = stack pointer
 * : upd = if 0, then the stack param will not be updated
 */
.macro save_fpu_inc tmp, stack, upd=0
	vmrs    \tmp, FPEXC
	stmia   \stack, {\tmp}
	tst     \tmp, #VFP_FPEXC_EN
	beq     1f
	add     \tmp, \stack, #4
	vstmia  \tmp!, {d0-d15}
	vstmia  \tmp, {d16-d31}
1:
.if \upd
	add     \stack, \stack, #(FPU_DATA_LEN * 4)
.endif
.endm

/**
 * Purpose
 * : load struct fpu_context from the stack
 *
 * Params
 * : tmp = temporary register
 * : stack = stack pointer
 * : upd = if 0, then the stack param will not be updated
 */
.macro load_fpu_inc tmp, stack, upd=0
	ldmia   \stack, {\tmp}
	vmsr    FPEXC, \tmp
	tst     \tmp, #VFP_FPEXC_EN
	beq     1f
	add     \tmp, \stack, #4
	vldmia  \tmp!, {d0-d15}
	vldmia  \tmp, {d16-d31}
1:
.if \upd
	add     \stack, \stack, #(FPU_DATA_LEN * 4)
.endif
.endm

.macro save_fpu_dec tmp, stack, upd=0
	sub     sp, sp, #(FPU_DATA_LEN * 4)
	save_fpu_inc \tmp, \stack, !\upd
.endm

.macro load_fpu_dec tmp, stack, upd=0
	sub     sp, sp, #(FPU_DATA_LEN * 4)
	load_fpu_inc \tmp, \stack, !\upd
.endm
