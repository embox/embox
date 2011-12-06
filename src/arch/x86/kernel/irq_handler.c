/**
 * @file
 * @brief
 *
 * @date 06.01.11
 * @author Anton Bondarev
 */

#include <types.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <asm/regs.h>
#include <asm/traps.h>
#include <asm/io.h>
#include <drivers/i8259.h>
#include <assert.h>

static int irqHandlerIns = 0;
static int irqHandlerOuts = 0;

void irq_handler(pt_regs_t *regs) {
	int irqn = regs->trapno - 0x20;
	/* Send an EOI (end of interrupt) signal to the PICs.
	   If this interrupt involved the slave. */
	interrupt_disable(irqn);
	assert(!critical_inside(CRITICAL_IRQ_LOCK));
	critical_enter(CRITICAL_IRQ_HANDLER);
	if (irqn > 7) {
		/* Send reset signal to slave. */
		out8(NON_SPEC_EOI, PIC2_COMMAND);
	}
	/* Send reset signal to master. (As well as slave, if necessary). */
	out8(NON_SPEC_EOI, PIC1_COMMAND);

++irqHandlerIns;

	irq_dispatch(irqn);

++irqHandlerOuts;

	interrupt_enable(irqn);
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
