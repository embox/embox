/**
 * @file
 *
 * @date 19.11.12
 * @author Ilia Vaprol
 */

#include <asm/ptrace.h>
#include <kernel/printk.h>

void trap_handler(struct pt_regs *regs) {
    unsigned int *r;
    r = &regs->gpr[0];
    printk(
            "\nTRAP[0x%X]:\n"
            " r0   %08X r1   %08X r2   %08X r3   %08X\n"
            " r4   %08X r5   %08X r6   %08X r7   %08X\n"
            " r8   %08X r9   %08X r10  %08X r11  %08X\n"
            " r12  %08X r13  %08X r14  %08X r15  %08X\n"
            " r16  %08X r17  %08X r18  %08X r19  %08X\n"
            " r20  %08X r21  %08X r22  %08X r23  %08X\n"
            " r24  %08X r25  %08X r26  %08X r27  %08X\n"
            " r28  %08X r29  %08X r30  %08X r31  %08X\n"
            " lr   %08X cr   %08X xer  %08X ctr  %08X\n"
            " srr0 %08X srr1 %08X",
            regs->trapno,
            r[0],  r[1],  r[2],  r[3],  r[4],  r[5],  r[6],  r[7],
            r[8],  r[9],  r[10], r[11], r[12], r[13], r[14], r[15],
            r[16], r[17], r[18], r[19], r[20], r[21], r[22], r[23],
            r[24], r[25], r[26], r[27], r[28], r[29], r[30], r[31],
            regs->lr, regs->cr, regs->xer, regs->ctr,
            regs->srr0, regs->srr1);
}
