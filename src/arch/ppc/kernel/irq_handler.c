/**
 * @file
 *
 * @date 19.11.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/irqctrl.h>
#include <kernel/critical.h>
#include <kernel/irq.h>

void irq_handler(unsigned int irq) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	irqctrl_disable(irq);

	irqctrl_clear(irq);

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
//		ipl_enable();

		irq_dispatch(irq);

//		ipl_disable();
	}
	irqctrl_enable(irq);
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
