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
 * : store/load struct fpu_context on/from the stack
 *
 * Params
 * : inst = ldm or stm
 * : tmp = temporary register
 * : stack = stack pointer
 * : upd = if 0, then the stack param will not be updated
 */
.macro _save_load_fpu inst, tmp, stack, upd
	ldr     \tmp, =FPU_CPACR
	ldr     \tmp, [\tmp]
	tst     \tmp, #0xf00000
	beq     1f
	add     \tmp, \stack, #4
	v\inst\()ia \tmp, {s0-s31}
1:
.if \upd
	add     sp, sp, #(FPU_DATA_LEN * 4)
.endif
.endm

.macro save_fpu_inc tmp, stack, upd=0
	_save_load_fpu stm \tmp, \stack, \upd
.endm

.macro load_fpu_inc tmp, stack, upd=0
	_save_load_fpu ldm \tmp, \stack, \upd
.endm

.macro save_fpu_dec tmp, stack, upd=0
	sub     sp, sp, #(FPU_DATA_LEN * 4)
	_save_load_fpu stm \tmp, \stack, !\upd
.endm

.macro load_fpu_dec tmp, stack, upd=0
	sub     sp, sp, #(FPU_DATA_LEN * 4)
	_save_load_fpu ldm \tmp, \stack, !\upd
.endm
