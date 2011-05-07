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


#define SET_HANDLER(base_addr, handler)            \
	addik reg_tmp1, reg_zero, handler;             \
	sw    reg_tmp1, reg_sp, reg_zero ;             \
	lhu   reg_tmp2, reg_sp, reg_zero;              \
	shi   reg_tmp2, reg_zero, (base_addr + 0x2) ;  \
	shi   reg_tmp1, reg_zero, (base_addr + 0x6) ;


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

	#if 0
/*now we will be set interrrupts and other vectors.
at the first we set 0xb000 0000 (code IMM instruction) and 0xb808 0000 (code BRAI instruction)
how describe in section "Reset, Interrupts, Exceptions, and Break" mb_ref_guide */
	/* add opcode instruction for 32bit jump - 2 instruction imm & brai*/

	addik	r6, r0, 0xb0000000      /* hex b000 opcode imm */
	swi	r6, r0, 0x0                 /* reset address */
	swi	r6, r0, 0x8                 /* user vector exception */
	swi	r6, r0, 0x10                /* interrupt */
	swi	r6, r0, 0x20                /* hardware exception */

	addik	r6, r0, 0xb8080000      /* hex b808 opcode brai*/
	swi	r6, r0, 0x4                 /* reset address */
	swi	r6, r0, 0xC                 /* user vector exception */
	swi	r6, r0, 0x14                /* interrupt */
	swi	r6, r0, 0x24                /* hardware exception */

	/* now we set real handler*/
	/*Reset/start*/
	SET_HANDLER(0x00, _start);
	/*User exception*/
	SET_HANDLER(0x08, _user_exception);
	/*Inerrupt*/
	SET_HANDLER(0x10, _interrupt_handler);
	/*Break (HW/SW) debug exception*/
	SET_HANDLER(0x18, _debug_exception);
	/*Hardware exception*/
	SET_HANDLER(0x20, _hw_exception_handler);
#endif

1:
	/* enable instruction and data cache */
	mfs	r12, rmsr
	ori	r12, r12, 0xa0
	mts	rmsr, r12


	/*call hardware_init_hook if it set */
	addik reg_tmp1, reg_zero, hardware_init_hook
	beqi reg_tmp1, 1f
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
