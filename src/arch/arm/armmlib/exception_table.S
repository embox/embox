/**
 * @file
 *
 * @date 28.03.10
 * @author Anton Kozlov

 */

#include <arm/fpu.h>
#include <arm/exception.h>
#include <asm/arm_m_regs.h>
#include <module/embox/kernel/irq_api.h>
#include <framework/mod/options.h>
#include <debug/gdbstub.h>

#define IRQ_STACK_SIZE OPTION_GET(NUMBER, irq_stack_size)
#define IRQ_STACK_IN_BSS OPTION_GET(NUMBER, irq_stack_in_bss)

.type start, %function
.type exp_default_entry, %function

.global start
.global exp_default_entry

.section .trap_table, "x"
	.word irq_stack_top
	.word start
	.word exc_entry2
	.word exc_entry3
	.word exc_entry4
	.word exc_entry5
	.word exc_entry6
	.word exc_entry7
	.word exc_entry8
	.word exc_entry9
	.word exc_entry10
	.word exc_entry11
	.word exc_entry12
	.word exc_entry13
	.word exc_entry14
	.word exc_entry15

.section .text.bootcode
.thumb
.syntax unified

exp_default_entry:
#if defined(__ARM_ARCH_6M__) /* FIXME unsupported 'tst' for Cortex-M0 */
	mov  r0, sp
	mrs  r1, IPSR
#else
	mrs r1, IPSR

	/* Registers are saved on MSP stack if the exception
	 * happened inside interrupt context. Otherwise, we are on PSP stack. */
	ldr r1, =SCB_ICSR
	ldr r0, [r1]
	tst r0, SCB_ICSR_RETTOBASE
	beq 1f
	mrs r0, psp
	b   2f
1:
	mrs r0, msp
2:
#endif
	bl  exc_default_handler

start:
	# Init PSP stack
	ldr r0, =_stack_top
	msr psp, r0

	# Switch to PSP stack
	mrs  r0, CONTROL
	movs r1, #0x2
	orrs r0, r0, r1
	msr  CONTROL, r0
	isb

	ldr r1, =bootldr_start
	bx r1
die:
	b 	    die

#if GDBSTUB_IMPL
.type EXC_HANDLER_NAME(DEBUG_MONITOR), %function
.global EXC_HANDLER_NAME(DEBUG_MONITOR)
EXC_HANDLER_NAME(DEBUG_MONITOR):
	mrs r1, IPSR
	ldr r1, =SCB_ICSR
	ldr r0, [r1]
	tst r0, SCB_ICSR_RETTOBASE
	beq 1f
	mrs r0, psp
	b   2f
1:
	mrs r0, msp
2:
	add r3, r0, #SAVED_CORE_REGS_SIZE
#ifdef ARM_FPU_VFP
	add r3, r3, #SAVED_FPU_REGS_SIZE
#endif
	ldr r1, [r0, #16]
	ldr r2, [r0, #24]
	ldr r12, [r0, #20]
	stmfd sp!, {r1-r3, r12}
	stmfd sp!, {r4-r11}
	ldmfd r0, {r0-r3}
	stmfd sp!, {r0-r3}
	mov r0, sp
	stmfd sp!, {lr}
	bl exc_debug_monitor
	ldmfd sp!, {lr}
	add sp, sp, #16
	ldmfd sp!, {r4-r11}
	add sp, sp, #16
	bx lr
#endif

#if IRQ_STACK_IN_BSS == 0
.data
.align 3
irq_stack_start:
	.rept   IRQ_STACK_SIZE
	.byte   0x50
	.endr
irq_stack_top:
#else
.bss
.align 3
irq_stack_start:
	.rept   IRQ_STACK_SIZE
	.byte 0x0
	.endr
irq_stack_top:
#endif
