/**
 * @brief Interrupt handler
 *
 * @date 15.12.22
 * @author Aleksey Zhmulin
 */
#include <assert.h>

#include <drivers/irqctrl.h>
#include <kernel/critical.h>
#include <kernel/irq.h>

#include "exception.h"

void aarch64_irq_handler(struct excpt_context *ctx) {
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
