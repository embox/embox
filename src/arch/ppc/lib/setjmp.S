/*
 * @file
 *
 * @date 17.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <asm/setjmp.h>

    .section .text

ENTRY setjmp
    mr   r9, r1            /* copy stack pointer to r9 */
    mr   r10, r2           /* copy table of contents pointer to r10 */
    mflr r11               /* copy link register to r11 *
    mfcr r12               /* copy condition register to r12 */
    stmw r9, SETJMP_SP(r3) /* save r9..r31 registers to first argument[struct __jmp_buf] */
    li   r3, 0             /* indicate success */
    blr                    /* return */

ENTRY longjmp
    lmw  r9, SETJMP_SP(r3)  /* load r9..r31 registers from first argument[struct __jmp_buf] */
    mtcr r12                /* copy condition register from r12 */
    mtlr r11                /* copy link register from r11 */
    mr   r2, r10            /* copy table of contents pointer from r10 */
    mr   r1, r9             /* copy stack pointer from r9 */
    or.  r3, r4, r4         /* test second argument[int] */
    bnelr                   /* return if not 0 */
    li   r3, 1              /* else set return value to 1 */
    blr                     /* return */

