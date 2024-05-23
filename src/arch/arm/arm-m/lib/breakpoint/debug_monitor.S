/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#include <arm/fpu.h>
#include <arm/exception.h>
#include <asm/arm_m_regs.h>

.section .text.bootcode
.thumb
.syntax unified

.type EXC_HANDLER_NAME(DEBUG_MONITOR), %function
.global EXC_HANDLER_NAME(DEBUG_MONITOR)

EXC_HANDLER_NAME(DEBUG_MONITOR):
	@ Move top of excpt stack to r0
	ldr     r1, =SCB_ICSR
	ldr     r0, [r1]
	tst     r0, #SCB_ICSR_RETTOBASE
	beq     1f
	mrs     r0, psp
	b       2f
1:
	mrs     r0, msp
2:
	@ Store lr, pc, PSR
	ldr     r1, [r0, #20]
	ldr     r2, [r0, #24]
	ldr     r3, [r0, #28]
	stmfd   sp!, {r1-r3}

	@ Store r12, sp
	ldr     r1, [r0, #16]
	add     r2, r0, #SAVED_CORE_REGS_SIZE
#ifdef ARM_FPU_VFP
	add     r2, r2, #SAVED_FPU_REGS_SIZE
#endif
	stmfd   sp!, {r1-r2}

	@ Store r0-r11
	stmfd   sp!, {r4-r11}
	ldmfd   r0, {r0-r3}
	stmfd   sp!, {r0-r3}

	@ Call arm_m_debug_monitor_handler
	mov     r0, sp
	stmfd   sp!, {lr}
	bl      arm_m_debug_monitor_handler
	ldmfd   sp!, {lr}

	@ Load regs and exit
	add     sp, sp, #16
	ldmfd   sp!, {r4-r11}
	add     sp, sp, #20
	bx      lr
