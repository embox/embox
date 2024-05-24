/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.05.24
 */

#include <assert.h>

#include <drivers/irqctrl.h>
#include <kernel/critical.h>
#include <kernel/irq.h>

void mips_interrupt_handler(void) {
	unsigned int irq;

	irq = irqctrl_get_intid();
	if (irq == -1) {
		return;
	}

	assert(irq_nr_valid(irq));
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	irqctrl_disable(irq);
	irqctrl_eoi(irq);
	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		ipl_enable();

		irq_dispatch(irq);

		ipl_disable();
	}
	irqctrl_enable(irq);
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
