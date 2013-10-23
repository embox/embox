/**
 * @file
 * @brief Low-level boot code for microblaze architecture.
 *
 * @date 25.11.09
 * @author Anton Bondarev
 */

#include <asm/linkage.h>
#include <asm/regs.h>

	.section .text

/**
 * Kernel start point
 */
C_ENTRY(_start):
	mts	rmsr, reg_zero	/* disable cache, interrupts, exceptions */

	/*set stack pointer*/
	addi  reg_sp, reg_zero, _stack_top

	brlid r15, setup_trap_table
	nop

	/* enable instruction and data cache */
	mfs	r12, rmsr
	ori	r12, r12, 0xa0
	mts	rmsr, r12

	/* clear BSS segments */
clear_bss:
#define cur_addr r11
#define end_addr r12
#define cmp_reg  r6
	addik  cur_addr, reg_zero, _bstart /*set start address for bss*/
	addik  end_addr, reg_zero, _bend   /*set end address for bss section*/
1:
	cmp   cmp_reg, cur_addr, end_addr
	beqi  cmp_reg, 2f                  /*cmp cur_addr and end_addr*/

	swi   reg_zero, cur_addr, 0        /* write zero to loc */
	addi  cur_addr, cur_addr, 4        /* increment to next loc */
	brai    1b
2:
	/*load data section*/
	addik r5, r0, _data_vma
	addik r6, r0, _data_lma
	cmp   r7, r6, r5
	beqi  r7, 1f
	addik r7, r0, _data_len

 	brlid r15, memcpy
	 nop
1:

	/*clear register*/
	REGS_INIT();
	/*set stack pointer again*/
	addi  reg_sp, reg_zero, _stack_top

	brlid r15, kernel_start
	nop
