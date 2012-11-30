/**
 * @file
 *
 * @date 16.11.12
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <hal/context.h>

    .section .text

ENTRY context_switch
    isync                     /* synchronize */
    stmw   r13, CTX_GPR13(r3) /* save r13..r31 to first argument[struct context] */
    mflr   r28                /* save lr, cr, sp, tc to first argument[struct context] */
    mfcr   r29
    mr     r30, r1
    mr     r31, r2
    stmw   r28, CTX_LR(r3)

    lmw    r28, CTX_LR(r4)    /* load lr, cr, sp, tc from second argument[struct context] */
    mtlr   r28
    mtcr   r29
    mr     r1, r30
    mr     r2, r31
    lmw    r13, CTX_GPR13(r4) /* load r13..r31 from second argument[struct context] */
    isync                     /* synchronize */

    blr                       /* return */
