/**
 * @file
 * @brief
 *
 * @date 06.01.11
 * @author Anton Bondarev
 */

#include <types.h>
//#include <hal/arch.h>
//#include <hal/interrupt.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <asm/regs.h>
#include <asm/traps.h>
#include <asm/io.h>
#include <drivers/apic.h>

void irq_handler(pt_regs_t regs) {
	int irqn = regs.trapno - 0x20;
	/* Send an EOI (end of interrupt) signal to the PICs.
	   If this interrupt involved the slave. */
	interrupt_disable(irqn);
	if (irqn > 7) {
		/* Send reset signal to slave. */
		out8(NON_SPEC_EOI, PIC2_COMMAND);
	}
	/* Send reset signal to master. (As well as slave, if necessary). */
	out8(NON_SPEC_EOI, PIC1_COMMAND);
#ifdef CONFIG_IRQ
	irq_dispatch(irqn);
#endif
	interrupt_enable(irqn);
}
