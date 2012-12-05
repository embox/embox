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
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <module/embox/arch/interrupt.h>

    .section .trap_table.text

/**
 * Exception table:
 * 0x00..0x04 - Reset handler
 * 0x08..0x0C - User Vector (Exception) handler
 * 0x10..0x14 - Interrupt handler
 * 0x18..0x1C - Break Non-maskable hardware/Hardware/Software handler
 * 0x20..0x24 - Hardware Exception handler
 * 0x28..0x4F - Reserved
 */

    .macro EXCEPTION_ENTRY hnd
		.align 3
        brai \hnd
    .endm

/**
 * Trap table start
 */
trap_table_start:

EXCEPTION_ENTRY reset_hnd           /* reset */
EXCEPTION_ENTRY user_except_hnd     /* user vector */

#ifndef INTERRUPT_STUB              /* interrupt */
EXCEPTION_ENTRY irq_hnd
#else /* INTERRUPT_STUB */
EXCEPTION_ENTRY die
#endif /* !INTERRUPT_STUB */

EXCEPTION_ENTRY break_except_hnd    /* break */
EXCEPTION_ENTRY hardware_except_hnd /* hardware exception */

/**
 * Trap table end
 */
trap_table_end:

die:
	brai die


/*******************************************/

	.section .text

/**
 * Copy trap table to 0x0
 */
C_ENTRY(setup_trap_table):
	addik r8, r15, 0               /* we don't use stack and must save r15 ourself */
	addik r6, r0, trap_table_start /* whether copy interrupt vector table ? */
	addik r5, r0, 0
	rsubik r7, r6, trap_table_end  /* calculate size of table */

	brlid r15, memcpy
	nop
	rtsd r8, 8
	nop
