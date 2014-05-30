/**
 * @file
 *
 * @date 08.11.12
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <asm/ptrace.h>
#include <module/embox/arch/interrupt.h>

    .section .trap_table.text

/**
 * IVORn    Interrupt type
 * -----------------------
 * IVOR0  - Critical Input
 * IVOR1  - Machine Check
 * IVOR2  - Data Storage
 * IVOR3  - Instruction Storage
 * IVOR4  - External Input
 * IVOR5  - Alignment
 * IVOR6  - Program
 * IVOR7  - Floating Point Unavailable
 * IVOR8  - System call
 * IVOR9  - Auxiliary Processor
 * IVOR10 - Decrementer
 * IVOR11 - Fixed Interval Timer
 * IVOR12 - Watchdog Timer
 * IVOR13 - Data TLB Error
 * IVOR14 - Instruction TLB Error
 * IVOR15 - Debug
 */

    .macro EXCEPTION_ENTRY n
        .align 4              /* XXX maybe it should be 5 */
        ENTRY except\n\()_hnd /* define entry for except%d_hnd */
    .endm

    .macro TRAP_ENTRY n
        EXCEPTION_ENTRY \n    /* define general entry point for traps */
            SAVE_ALL          /* save all registers */
            li r0, \n         /* save trap's type into r0 */
            b trap_stub       /* call default trap handler */
    .endm

    .macro IRQ_ENTRY n
#ifndef INTERRUPT_STUB
        EXCEPTION_ENTRY \n    /* define general entry point for interrupts */
            SAVE_ALL          /* save all register */
            li r0, \n         /* load interrupt type into r0 */
            b irq_stub        /* call default interrupt handler */
#else /* INTERRUPT_STUB */
        TRAP_ENTRY \n         /* handle irq as trap */
#endif /* !INTERRUPT_STUB */
    .endm

/**
 * Align trap table because all excepts handler must have the same HI bytes
 */
.align 8

TRAP_ENTRY 0  /* Critical Input interrupt handler */
IRQ_ENTRY  1  /* Machine Check interrupt handler */
TRAP_ENTRY 2  /* Data Storage interrupt handler */
TRAP_ENTRY 3  /* Instruction Storage interrupt handler */
IRQ_ENTRY  4  /* External Input interrupt handler */
TRAP_ENTRY 5  /* Alignment interrupt handler */
TRAP_ENTRY 6  /* Program interrupt handler */
TRAP_ENTRY 7  /* Floating Point Unavailable interrupt handler */
TRAP_ENTRY 8  /* System call interrupt handler */
TRAP_ENTRY 9  /* Auxiliary Processor interrupt handler */
IRQ_ENTRY  10 /* Decrementer interrupt handler */
TRAP_ENTRY 11 /* Fixed Interval Timer interrupt handler */
TRAP_ENTRY 12 /* Watchdog Timer interrupt handler */
TRAP_ENTRY 13 /* Data TLB Error interrupt handler */
TRAP_ENTRY 14 /* Instruction TLB Error interrupt handler */
TRAP_ENTRY 15 /* Debug interrupt handler */
