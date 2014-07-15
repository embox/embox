/**
 * @file
 * @brief Low-level interrupt handler
 *
 * @date 12.11.12
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <asm/ptrace.h>

    .section .text

ENTRY irq_stub
    subi r1, r1, 8   /* correct stack before call C code */
    mr   r3, r0      /* set interrupt type as first argument */
    bl   irq_handler /* handle interrupt */
    addi r1, r1, 8   /* restore stack pointer */
    RESTORE_ALL      /* restore all registers */
    rfi              /* return from interrupt */

die:                 /* infinite loop */
    b die
