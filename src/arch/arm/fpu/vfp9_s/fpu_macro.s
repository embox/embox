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

.macro _save_load_fpu inst, tmp, stack, upd, force
.if \inst == stm
	vmrs    \tmp, FPEXC
.endif
	\inst\()ia \stack, {\tmp}
.if \inst == ldm
	vmsr    FPEXC, \tmp
.endif
.if !\force
	tst     \tmp, #(1 << 30)
	beq     1f
.endif
	add     \tmp, \stack, #4
	v\inst\()ia \tmp, {d0-d15}
1:
.if \upd
	add     sp, sp, #(FPU_DATA_LEN * 4)
.endif
.endm

.macro save_fpu_inc tmp, stack, upd=0, force=0
	_save_load_fpu stm \tmp, \stack, \upd \force
.endm

.macro load_fpu_inc tmp, stack, upd=0, force=0
	_save_load_fpu ldm \tmp, \stack, \upd \force
.endm

.macro save_fpu_dec tmp, stack, upd=0, force=0
	sub     sp, sp, #(FPU_DATA_LEN * 4)
	_save_load_fpu stm \tmp, \stack, !\upd \force
.endm

.macro load_fpu_dec tmp, stack, upd=0, force=0
	sub     sp, sp, #(FPU_DATA_LEN * 4)
	_save_load_fpu ldm \tmp, \stack, !\upd \force
.endm
