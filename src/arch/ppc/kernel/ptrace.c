/**
 * @file
 *
 * @date 19.11.12
 * @author Ilia Vaprol
 */

#include <asm/ptrace.h>
#include <prom/prom_printf.h>


void ptrace_info(struct pt_regs *regs) {
	prom_printf("\nPTRACE_INFO: regs[%p]\n", regs);
	prom_printf(" r0  %08X; r1  %08X; r2  %08X; r3  %08X;\n",
			regs->gpr[0], regs->gpr[1], regs->gpr[2], regs->gpr[3]);
	prom_printf(" r4  %08X; r5  %08X; r6  %08X; r7  %08X;\n",
			regs->gpr[4], regs->gpr[5], regs->gpr[6], regs->gpr[7]);
	prom_printf(" r8  %08X; r9  %08X; r10 %08X; r11 %08X;\n",
			regs->gpr[8], regs->gpr[9], regs->gpr[10], regs->gpr[11]);
	prom_printf(" r12 %08X; r13 %08X; r14 %08X; r15 %08X;\n",
			regs->gpr[12], regs->gpr[13], regs->gpr[14], regs->gpr[15]);
	prom_printf(" r16 %08X; r17 %08X; r18 %08X; r19 %08X;\n",
			regs->gpr[16], regs->gpr[17], regs->gpr[18], regs->gpr[19]);
	prom_printf(" r20 %08X; r21 %08X; r22 %08X; r23 %08X;\n",
			regs->gpr[20], regs->gpr[21], regs->gpr[22], regs->gpr[23]);
	prom_printf(" r24 %08X; r25 %08X; r26 %08X; r27 %08X;\n",
			regs->gpr[24], regs->gpr[25], regs->gpr[26], regs->gpr[27]);
	prom_printf(" r28 %08X; r29 %08X; r30 %08X; r31 %08X;\n",
			regs->gpr[28], regs->gpr[29], regs->gpr[30], regs->gpr[31]);
	prom_printf(" lr  %08X; cr  %08X; xer %08X; ctr %08X;\n",
			regs->lr, regs->cr, regs->xer, regs->ctr);
	prom_printf(" srr0 %08X    srr1 %08X\n", regs->srr0, regs->srr1);
}
