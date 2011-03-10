/**
 * @file
 * @brief Interrupt vector table for Microblaze arcitecture
 *
 * @date 25.11.09
 * @author Anton Bondarev
 */
#include <asm/linkage.h>

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
