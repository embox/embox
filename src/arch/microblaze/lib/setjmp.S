/**
 * @file
 *
 * @date 02.11.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <asm/setjmp.h>

    .section .text

C_ENTRY(setjmp):
    /* save all into first argument[struct __jmp_buf] - r5 register */
    swi  r1, r5, SETJMP_SP    /* save stack pointer */
    swi  r2, r5, SETJMP_RSDA  /* save read-only sda register */
    swi  r13, r5, SETJMP_WSDA /* save read-write sda register */
    swi  r15, r5, SETJMP_RA   /* save link register */
    swi  r18, r5, SETJMP_RZRV /* save reserved for assembler register */
    swi  r19, r5, SETJMP_S19  /* save non-volatile r19..r31 registers */
    swi  r20, r5, SETJMP_S20
    swi  r21, r5, SETJMP_S21
    swi  r22, r5, SETJMP_S22
    swi  r23, r5, SETJMP_S23
    swi  r24, r5, SETJMP_S24
    swi  r25, r5, SETJMP_S25
    swi  r26, r5, SETJMP_S26
    swi  r27, r5, SETJMP_S27
    swi  r28, r5, SETJMP_S28
    swi  r29, r5, SETJMP_S29
    swi  r30, r5, SETJMP_S30
    swi  r31, r5, SETJMP_S31
    or   r3, r0, r0           /* indicate success */
    rtsd r15, 8               /* normal return */
    nop

C_ENTRY(longjmp):
    /* load all from first argument[struct __jmp_buf] - r5 register */
    lwi  r1, r5, SETJMP_SP    /* load stack pointer */
    lwi  r2, r5, SETJMP_RSDA  /* load read-only sda register */
    lwi  r13, r5, SETJMP_WSDA /* load read-write sda register */
    lwi  r15, r5, SETJMP_RA   /* load link register */
    lwi  r18, r5, SETJMP_RZRV /* load reserved for assembler rgister */
    lwi  r19, r5, SETJMP_S19  /* load non-volatile r19..r31 registers */
    lwi  r20, r5, SETJMP_S20
    lwi  r21, r5, SETJMP_S21
    lwi  r22, r5, SETJMP_S22
    lwi  r23, r5, SETJMP_S23
    lwi  r24, r5, SETJMP_S24
    lwi  r25, r5, SETJMP_S25
    lwi  r26, r5, SETJMP_S26
    lwi  r27, r5, SETJMP_S27
    lwi  r28, r5, SETJMP_S28
    lwi  r29, r5, SETJMP_S29
    lwi  r30, r5, SETJMP_S30
    lwi  r31, r5, SETJMP_S31
    or   r3, r6, r0           /* set and test return value using second argument[int] */
    bnei r3, 1f               /* branch to return if not equal to zero */
    addi r3, r0, 1            /* else set return value to 1 */
1:
    rtsd    r15, 8            /* normal return */
    nop
