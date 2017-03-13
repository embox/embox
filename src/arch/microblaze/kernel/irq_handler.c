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

extern unsigned int irqctrl_get_irq_num(void);
/* we havn't interrupts acknowledgment in microblaze architecture
 * and must receive interrupt number our self and then clear pending bit in
 * pending register
 */
void irq_handler(void) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		unsigned int irq_num;
		irq_num = irqctrl_get_irq_num();

		if (irqctrl_pending(irq_num)) {
			/* disable interrupt and clear it later, since ACK
			 * have no effect on level interrupt */
			irqctrl_disable(irq_num);

			/*now we allow nested IRQ*/
			ipl_enable();

			irq_dispatch(irq_num);

			ipl_disable();

			/* clear interrupt, for level interrupts it's dispatcher
			* should set line low, edge gets it's ACK too */
			/* FIXME possible miss of edge interrupt happened while in self
			* dispatcher */
			irqctrl_clear(irq_num);

			irqctrl_enable(irq_num);
		}
	}

	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
