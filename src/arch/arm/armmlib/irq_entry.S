/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    25.05.2020
 */

#include <arm/fpu.h>
#include <arm/exception.h>
#include <module/embox/kernel/irq_api.h>
#include <asm/arm_m_regs.h>

#if !defined(__ARM_ARCH_6M__) /* FIXME unsupported for Cortex-M0 */

.text
.thumb
.syntax unified

#define PSR_RESET_VALUE 0x01000000
#define SAVED_FPU_REGS_SIZE  (16 * 4 + 4 + 4)
#define SAVED_CORE_REGS_SIZE (8 * 4)

#ifndef STATIC_IRQ_EXTENTION
.thumb_func
.global arm_m_irq_entry
arm_m_irq_entry:
	push {lr}
	bl nvic_irq_handle
	pop {lr}
	b arm_m_irq_exit
#endif

.thumb_func
.global arm_m_irq_exit
arm_m_irq_exit:
	/* Return if we are in nested intertupt */
	ldr r1, =SCB_ICSR
	ldr r0, [r1]
	tst r0, SCB_ICSR_RETTOBASE
	beq 1f

	/* Return if we already requested PendSV */
	tst r0, SCB_ICSR_PENDSVSET
	bne 1f

	/* Return if there is no work pending */
	push {lr}
	bl critical_dispatch_required
	pop {lr}
	cmp r0, #0
	beq 1f

	/* Prepare context to exit to arm_m_dispatch_pending() */
#ifdef ARM_FPU_VFP
	sub sp, #SAVED_FPU_REGS_SIZE
#endif
	mov r1, #PSR_RESET_VALUE
	push {r1}                      /* xPSR */
	ldr r1, =arm_m_dispatch_pending
	push {r1}                      /* pc */
	push {r1}                      /* lr (equal to pc here) */
	sub  sp, #(4 * 5)              /* r12, r0-r4 */

1:
	bx lr

arm_m_dispatch_pending:
	bl critical_dispatch_pending

	mov r0, #SCB_ICSR_PENDSVSET
	ldr r1, =SCB_ICSR               /* Raise PendSV */
	str r0, [r1]

	/* No return here, we return through PendSV handler. */
2:
	b 2b

.thumb_func
.global EXC_HANDLER_NAME(PENDSV_IRQ)
EXC_HANDLER_NAME(PENDSV_IRQ):
	/* Skip PendSV context */
#ifdef ARM_FPU_VFP
	add sp, #SAVED_FPU_REGS_SIZE
#endif
	add sp, #SAVED_CORE_REGS_SIZE
	/* Finally, restore context previously CPU-saved on arm_m_irq_entry() entering */
	bx lr
#endif /* !defined(__ARM_ARCH_6M__) */
