#include <asm/linkage.h>
#include <asm/mb_regs.h>

	.section ".trap_table"
	.balign 0x8
C_ENTRY(_start):
	brai boot_start
	.balign 0x8
	brai _user_exception
	.balign 0x8
	brai _interrupt_handler
	.balign 0x8
	brai _debug_exception
	.balign 0x8
	brai _hw_exception_handler


	.section ".text"

C_ENTRY(_hardreset):
	brai boot_start
	nop
	nop

C_ENTRY(_interrupt_handler):
#if 0
//FIXME Microblaze traps table code
	SAVE_REGS

	brlid	r15, interrupt_handler
	nop
	nop
	RESTORE_REGS

/*	nop
	addi	r1, r1, -4
	swi	r12, r1, 0
	mfs	r12, rmsr
	ori	r12, r12, 2
	mts	rmsr, r12
	lwi	r12, r1, 0
	addi	r1, r1, 4
	nop
	*/
	msrclr -1
	msrset	r0, 2

	bra	 r14
	nop
	nop

#else

	addi	r1, r1, -4
	swi	r2, r1, 0
	addi	r1, r1, -4
	swi	r3, r1, 0
	addi	r1, r1, -4
	swi	r4, r1, 0
	addi	r1, r1, -4
	swi	r5, r1, 0
	addi	r1, r1, -4
	swi	r6, r1, 0
	addi	r1, r1, -4
	swi	r7, r1, 0
	addi	r1, r1, -4
	swi	r8, r1, 0
	addi	r1, r1, -4
	swi	r9, r1, 0
	addi	r1, r1, -4
	swi	r10, r1, 0
	addi	r1, r1, -4
	swi	r11, r1, 0
	addi	r1, r1, -4
	swi	r12, r1, 0
	addi	r1, r1, -4
	swi	r13, r1, 0
	addi	r1, r1, -4
	swi	r14, r1, 0
	addi	r1, r1, -4
	swi	r15, r1, 0
	addi	r1, r1, -4
	swi	r16, r1, 0
	addi	r1, r1, -4
	swi	r17, r1, 0
	addi	r1, r1, -4
	swi	r18, r1, 0
	addi	r1, r1, -4
	swi	r19, r1, 0
	addi	r1, r1, -4
	swi	r20, r1, 0
	addi	r1, r1, -4
	swi	r21, r1, 0
	addi	r1, r1, -4
	swi	r22, r1, 0
	addi	r1, r1, -4
	swi	r23, r1, 0
	addi	r1, r1, -4
	swi	r24, r1, 0
	addi	r1, r1, -4
	swi	r25, r1, 0
	addi	r1, r1, -4
	swi	r26, r1, 0
	addi	r1, r1, -4
	swi	r27, r1, 0
	addi	r1, r1, -4
	swi	r28, r1, 0
	addi	r1, r1, -4
	swi	r29, r1, 0
	addi	r1, r1, -4
	swi	r30, r1, 0
	addi	r1, r1, -4
	swi	r31, r1, 0
	brlid	r15, interrupt_handler
	nop
	nop
	lwi	r31, r1, 0
	addi	r1, r1, 4
	lwi	r30, r1, 0
	addi	r1, r1, 4
	lwi	r29, r1, 0
	addi	r1, r1, 4
	lwi	r28, r1, 0
	addi	r1, r1, 4
	lwi	r27, r1, 0
	addi	r1, r1, 4
	lwi	r26, r1, 0
	addi	r1, r1, 4
	lwi	r25, r1, 0
	addi	r1, r1, 4
	lwi	r24, r1, 0
	addi	r1, r1, 4
	lwi	r23, r1, 0
	addi	r1, r1, 4
	lwi	r22, r1, 0
	addi	r1, r1, 4
	lwi	r21, r1, 0
	addi	r1, r1, 4
	lwi	r20, r1, 0
	addi	r1, r1, 4
	lwi	r19, r1, 0
	addi	r1, r1, 4
	lwi	r18, r1, 0
	addi	r1, r1, 4
	lwi	r17, r1, 0
	addi	r1, r1, 4
	lwi	r16, r1, 0
	addi	r1, r1, 4
	lwi	r15, r1, 0
	addi	r1, r1, 4
	lwi	r14, r1, 0
	addi	r1, r1, 4
	lwi	r13, r1, 0
	addi	r1, r1, 4
	lwi	r12, r1, 0
	addi	r1, r1, 4
	lwi	r11, r1, 0
	addi	r1, r1, 4
	lwi	r10, r1, 0
	addi	r1, r1, 4
	lwi	r9, r1, 0
	addi	r1, r1, 4
	lwi	r8, r1, 0
	addi	r1, r1, 4
	lwi	r7, r1, 0
	addi	r1, r1, 4
	lwi	r6, r1, 0
	addi	r1, r1, 4
	lwi	r5, r1, 0
	addi	r1, r1, 4
	lwi	r4, r1, 0
	addi	r1, r1, 4
	lwi	r3, r1, 0
	addi	r1, r1, 4
	lwi	r2, r1, 0
	addi	r1, r1, 4

	/* enable_interrupt */
//#ifdef XILINX_USE_MSR_INSTR
	msrset	r0, 2
//#else
	/* FIXME unstable in stressed mode - two irqs */
/*	nop
	addi	r1, r1, -4
	swi	r12, r1, 0
	mfs	r12, rmsr
	ori	r12, r12, 2
	mts	rmsr, r12
	lwi	r12, r1, 0
	addi	r1, r1, 4
	nop
#endif
*/
	bra	r14
	nop
	nop
#endif

C_ENTRY(_user_exception):
C_ENTRY(_debug_exception):
#if 1
	addi	r1, r1, -4
	swi	r2, r1, 0
	addi	r1, r1, -4
	swi	r3, r1, 0
	addi	r1, r1, -4
	swi	r4, r1, 0
	addi	r1, r1, -4
	swi	r5, r1, 0
	addi	r1, r1, -4
	swi	r6, r1, 0
	addi	r1, r1, -4
	swi	r7, r1, 0
	addi	r1, r1, -4
	swi	r8, r1, 0
	addi	r1, r1, -4
	swi	r9, r1, 0
	addi	r1, r1, -4
	swi	r10, r1, 0
	addi	r1, r1, -4
	swi	r11, r1, 0
	addi	r1, r1, -4
	swi	r12, r1, 0
	addi	r1, r1, -4
	swi	r13, r1, 0
	addi	r1, r1, -4
	swi	r14, r1, 0
	addi	r1, r1, -4
	swi	r15, r1, 0
	addi	r1, r1, -4
	swi	r16, r1, 0
	addi	r1, r1, -4
	swi	r17, r1, 0
	addi	r1, r1, -4
	swi	r18, r1, 0
	addi	r1, r1, -4
	swi	r19, r1, 0
	addi	r1, r1, -4
	swi	r20, r1, 0
	addi	r1, r1, -4
	swi	r21, r1, 0
	addi	r1, r1, -4
	swi	r22, r1, 0
	addi	r1, r1, -4
	swi	r23, r1, 0
	addi	r1, r1, -4
	swi	r24, r1, 0
	addi	r1, r1, -4
	swi	r25, r1, 0
	addi	r1, r1, -4
	swi	r26, r1, 0
	addi	r1, r1, -4
	swi	r27, r1, 0
	addi	r1, r1, -4
	swi	r28, r1, 0
	addi	r1, r1, -4
	swi	r29, r1, 0
	addi	r1, r1, -4
	swi	r30, r1, 0
	addi	r1, r1, -4
	swi	r31, r1, 0
	brlid	r15, exception_handler
	nop
	nop
	lwi	r31, r1, 0
	addi	r1, r1, 4
	lwi	r30, r1, 0
	addi	r1, r1, 4
	lwi	r29, r1, 0
	addi	r1, r1, 4
	lwi	r28, r1, 0
	addi	r1, r1, 4
	lwi	r27, r1, 0
	addi	r1, r1, 4
	lwi	r26, r1, 0
	addi	r1, r1, 4
	lwi	r25, r1, 0
	addi	r1, r1, 4
	lwi	r24, r1, 0
	addi	r1, r1, 4
	lwi	r23, r1, 0
	addi	r1, r1, 4
	lwi	r22, r1, 0
	addi	r1, r1, 4
	lwi	r21, r1, 0
	addi	r1, r1, 4
	lwi	r20, r1, 0
	addi	r1, r1, 4
	lwi	r19, r1, 0
	addi	r1, r1, 4
	lwi	r18, r1, 0
	addi	r1, r1, 4
	lwi	r17, r1, 0
	addi	r1, r1, 4
	lwi	r16, r1, 0
	addi	r1, r1, 4
	lwi	r15, r1, 0
	addi	r1, r1, 4
	lwi	r14, r1, 0
	addi	r1, r1, 4
	lwi	r13, r1, 0
	addi	r1, r1, 4
	lwi	r12, r1, 0
	addi	r1, r1, 4
	lwi	r11, r1, 0
	addi	r1, r1, 4
	lwi	r10, r1, 0
	addi	r1, r1, 4
	lwi	r9, r1, 0
	addi	r1, r1, 4
	lwi	r8, r1, 0
	addi	r1, r1, 4
	lwi	r7, r1, 0
	addi	r1, r1, 4
	lwi	r6, r1, 0
	addi	r1, r1, 4
	lwi	r5, r1, 0
	addi	r1, r1, 4
	lwi	r4, r1, 0
	addi	r1, r1, 4
	lwi	r3, r1, 0
	addi	r1, r1, 4
	lwi	r2, r1, 0
	addi	r1, r1, 4

	/* enable_interrupt */
//#ifdef XILINX_USE_MSR_INSTR
//	msrset	r0, 2
//#else
	/* FIXME unstable in stressed mode - two irqs */
/*	nop
	addi	r1, r1, -4
	swi	r12, r1, 0
	mfs	r12, rmsr
	ori	r12, r12, 2
	mts	rmsr, r12
	lwi	r12, r1, 0
	addi	r1, r1, 4
	nop
#endif
*/
	//rtsd r15,4
		msrset	r0, 8
	nop
	nop
	nop

	rtbd r16,8
	nop
	nop
#else
	brai boot_start
	nop
	nop
#endif

/*C_ENTRY(_debug_exception):
	brai boot_start
	nop
	nop
*/
C_ENTRY(_hw_exception_handler):
	brai boot_start
	nop
	nop


