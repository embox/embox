/**
 * @file
 *
 * @date May 9, 2018
 * @author Anton Bondarev
 */
#include <hal/cache.h>
#include <asm/regs.h>
#include <hal/reg.h>
#include <framework/mod/options.h>

/* OPTION_GET(NUMBER, irq_stack_size) */
#define IRQ_STACK_SIZE 0x100

.text
.global start
.global reset_handler
start:
reset_handler:
	/* Check if bootloader left MMU on; if so, it's neccessary to flush cache
	 * to make sure all the data is actually in RAM, not in some buffers */
	mrc     p15, 0, r0, c1, c0, 0
	and     r0, r0, #SCTLR_M
	cmp     r0, #0
	beq     cpu_modes_prepare
	ldr     r0, =_data_lma
	ldr     r1, =_data_len
	bl      dcache_flush
	ldr     r0, =_rodata_lma
	ldr     r1, =_rodata_len
	bl      dcache_flush
	ldr     r0, =_text_lma
	ldr     r1, =_text_len
	bl      dcache_flush

cpu_modes_prepare:
	/* Turn off MMU, instruction prediction and caches */
	mrc     p15, 0, r0, c1, c0, 0
	and     r0, r0, #~SCTLR_M
	and     r0, r0, #~SCTLR_W
	and     r0, r0, #~SCTLR_I
	and     r0, r0, #~SCTLR_D
	and     r0, r0, #~SCTLR_Z
	and     r0, r0, #~SCTLR_C
	mcr     p15, 0, r0, c1, c0, 0

	ldr     r0, =_stack_top

	/* Set stack pointer same for all modes and disable interrupts in all modes */
	msr     CPSR_c, #PSR_M_MON | PSR_I | PSR_F
	mov     sp, r0
	msr     CPSR_c, #PSR_M_FIQ | PSR_I | PSR_F
	mov     sp, r0
	msr     CPSR_c, #PSR_M_SVC | PSR_I | PSR_F
	mov     sp, r0
	msr     CPSR_c, #PSR_M_ABT | PSR_I | PSR_F
	mov     sp, r0
	msr     CPSR_c, #PSR_M_UND | PSR_I | PSR_F
	mov     sp, r0
	msr     CPSR_c, #PSR_M_IRQ | PSR_I | PSR_F
	mov     sp, r0

	/* put chip in System mode - move stack pointer down by IRQ_STACK_SIZE to make
	 * room for stack in above modes
	 */
	msr     CPSR_c, #PSR_M_SYS | PSR_I | PSR_F
	sub     r0, r0, #IRQ_STACK_SIZE
	mov     sp, r0

	bl      setup_excpt_table

	b       bootldr_start
