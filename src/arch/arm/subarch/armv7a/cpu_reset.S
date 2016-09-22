/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.10.2014
 */

#include <asm/regs.h>

.extern arm_trap_table

.global hardware_init_hook
hardware_init_hook:
	/* setup interrupt vector */
	ldr r0, =arm_trap_table
	mcr p15, 0, r0, c12, c0, 0

	/* setup c1, Control Register */
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #CR_A /* enabling data abort on unaligned mem access */
	orr r0, r0, #CR_C /* enabling data caching */
	orr r0, r0, #CR_Z /* enabling program flow prediction */
	orr r0, r0, #CR_I /* enabling instruction caching */
	mcr p15, 0, r0, c1, c0, 0

	/* setup c1, Auxiliary Control Register */
	mrc p15, 0, r0, c1, c0, 1
	orr r0, r0, #ACR_L2EN /* enabling L2 cache */
	mcr p15, 0, r0, c1, c0, 1

	mov pc, lr

