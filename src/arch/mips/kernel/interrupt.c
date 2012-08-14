/**
 * @file
 *
 * @brief
 *
 * @date 07.08.2012
 * @author Anton Bondarev
 */
#include <asm/mipsregs.h>
#include <kernel/irq.h>
#include <asm/ptrace.h>

void mips_c_interrupt_handler(pt_regs_t *regs) {
	unsigned int pending = mips_read_c0_cause() & mips_read_c0_status() & ST0_IM;
	int irq;
	for(irq = 15; irq > 7; irq--) {
		if(pending & (1 << irq)) {
			irq_dispatch(irq - 8);
		}
	}
}

