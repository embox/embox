/**
 * @file
 * @brief Low-level boot code for microblaze architecture.
 *
 * @date 25.11.09
 * @author Anton Bondarev
 */

#include <asm/linkage.h>
#include <asm/regs.h>

	.section    ".text"
	.align  4
 	.weak hardware_init_hook
 	.weak software_init_hook


C_ENTRY(boot_start):
	mts	rmsr, reg_zero	/* disable cache, interrupts, exceptions */

	addik r6, r0, _start /* whether copy interrupt vector table ? */
	beqi  r6, 1f
	nop
	/*set stack pointer*/
	addi  reg_sp, reg_zero, _stack

set_trap_table:
	addik r5, r0, 0
	addik r6, r0, _ivt_start
	addik r7, r0, 0x100


	brlid r15, memcpy
	nop

1:
	/* enable instruction and data cache */
	mfs	r12, rmsr
	ori	r12, r12, 0xa0
	mts	rmsr, r12


	/*call hardware_init_hook if it set */
	addik reg_tmp1, reg_zero, hardware_init_hook
	beqi reg_tmp1, clear_bss
	brlid r15, hardware_init_hook
	nop

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
	nop
2:

	/*set stack pointer*/
	addi  reg_sp, reg_zero, _stack

	/*load data section*/
	addik r5, r0, _data_vma
	addik r6, r0, _data_lma
	cmp   r7, r6, r5
	beqi  r7, 1f
	 nop
	addik r7, r0, _data_len

 	brlid r15, memcpy
	nop
1:
	/*interupts enabled*/
	mfs	r12, rmsr
	ori	r12, r12, 0x2
	mts	rmsr, r12

	/*clear register*/
	REGS_INIT();
	/*set stack pointer*/
	addi  reg_sp, reg_zero, _stack

1:
	/*call software_init_hook if it set */
	addik reg_tmp1, reg_zero, software_init_hook
	beqi reg_tmp1, 2f
	brai software_init_hook
	nop
2:
	brlid r15, kernel_start
	nop


	/*unreacheble loop*/
	//TODO may be it must be exeption or reset?
3:	bri	3b

	.section    ".text"
	.align  4
