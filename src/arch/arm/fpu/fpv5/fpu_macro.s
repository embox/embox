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
	ldr     \tmp, =FPU_CPACR
	ldr     \tmp, [\tmp]
.if !\force
	tst     \tmp, #0xf00000
	beq     1f
.endif
	add     \tmp, \stack, #4
	v\inst\()ia \tmp, {s0-s31}
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
