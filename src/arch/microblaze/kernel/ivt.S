/**
 * @file
 * @brief Interrupt vector table (ivt) for Microblaze architecture
 * @details Interrupt vector table (ivt) in Microblaze architcture must place
 *          at zero address. There are internal RAM in microblaze processor
 *          during power on there is internal boot code, but you must copy
 *          own ivt thet place.
 *
 * @date 25.11.09
 * @author Anton Bondarev
 */

#include <asm/linkage.h>
/* this section must occupate segment starting from zero address */
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
C_ENTRY(setup_trap_table):
	addik r8, r15, 0    /*we don't use stack and must save r15 ourself */
	addik r6, r0, _start /* whether copy interrupt vector table ? */
	beqi  r6, 1f
	nop

	addik r5, r0, 0
	addik r7, r0, 0x100

	brlid r15, memcpy
	nop
1:
	rtsd r8, 8
	nop
