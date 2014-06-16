/**
 * @file
 * @brief Low-level trap handler
 *
 * @date 12.11.12
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <asm/ptrace.h>

    .section .text

ENTRY trap_stub
    stw  r0, PT_REGS_TRAPNO(r1) /* save type of the trap into pt_regs */
    mr   r3, r1                 /* set pointer to pt_regs as first argument */
    subi r1, r1, 8              /* correct stack pointer */
    bl   trap_handler           /* handle trap */
    addi r1, r1, 8              /* restore stack pointer */
    b    die                    /* and jump to die */

die:                            /* infinite loop */
    b die
