/**
 * @file
 *
 * @date May 9, 2018
 * @author Anton Bondarev
 */
#include <arch/generic/dcache.h>
#include <asm/modes.h>
#include <asm/regs.h>
#include <asm/cp15.h>
#include <framework/mod/options.h>

/* OPTION_GET(NUMBER,irq_stack_size) */
#define IRQ_STACK_SIZE            0x100

.text
.global start
.global reset_handler
start:
reset_handler:
	/* Check if bootloader left MMU on; if so, it's neccessary to flush cache
	 * to make sure all the data is actually in RAM, not in some buffers */
	mrc	p15, 0, r0, c1, c0, 0
	and	r0, r0, #CR_M
	cmp	r0, #0
	beq	cpu_modes_prepare
	ldr	r0, =_data_lma
	ldr	r1, =_data_len
	bl	dcache_flush
	ldr	r0, =_rodata_lma
	ldr	r1, =_rodata_len
	bl	dcache_flush
	ldr	r0, =_text_lma
	ldr	r1, =_text_len
	bl	dcache_flush

cpu_modes_prepare:
	/* Turn off MMU, instruction prediction and caches */
	mrc p15, 0, r0, c1, c0, 0
	and r0, r0, #~CR_M
	and r0, r0, #~CR_W
	and r0, r0, #~CR_I
	and r0, r0, #~CR_D
	and r0, r0, #~CR_Z
	and r0, r0, #~CR_C
	mcr p15, 0, r0, c1, c0, 0

	ldr r0, =_stack_top

	/* Set stack pointer same for all modes and disable interrupts in all modes */
	msr CPSR_c, #ARM_MODE_MON | I_BIT | F_BIT
	mov sp, r0
	msr CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT
	mov sp, r0
	msr CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT
	mov sp, r0
	msr CPSR_c, #ARM_MODE_ABT | I_BIT | F_BIT
	mov sp, r0
	msr CPSR_c, #ARM_MODE_UND | I_BIT | F_BIT
	mov sp, r0
	msr CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
	mov sp, r0

	/* put chip in System mode - move stack pointer down by IRQ_STACK_SIZE to make 
	 * room for stack in above modes 
	 */
	msr CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT
	sub r0, r0, #IRQ_STACK_SIZE
	mov sp, r0

	bl setup_excpt_table

	b bootldr_start
