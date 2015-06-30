/**
 * @file
 * @details This file contains @link interrupt_handler() @endlink function.
 *        It's proxy between asm code and kernel interrupt handler
 *        @link irq_dispatch() @endlink function.
 *
 * @date 27.11.09
 * @author Anton Bondarev
 */

#include <assert.h>

#include <drivers/irqctrl.h>
#include <kernel/irq.h>

/* we havn't interrupts acknowledgment in microblaze architecture
 * and must receive interrupt number our self and then clear pending bit in
 * pending register
 */
void irq_handler(void) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		unsigned int irq_num;
		for (irq_num = 0; irq_num < IRQCTRL_IRQS_TOTAL; irq_num++) {
			if (irqctrl_pending(irq_num)) {
				/* disable interrupt and clear it later, since ack
					 * have no effect on level interrupt */
				irqctrl_disable(irq_num);

				/*now we allow nested irq*/
				ipl_enable();

				irq_dispatch(irq_num);

				ipl_disable();

				/* clear interrupt, for level interrupts it's dispatcher
				* should set line low, edge gets it's ack too */
				/* FIXME possible miss of edge interrupt happend while in self
				* dispatcher */
				irqctrl_clear(irq_num);

				irqctrl_enable(irq_num);
			}
		}
	}

	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
