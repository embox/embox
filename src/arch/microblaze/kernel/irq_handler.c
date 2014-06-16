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
#include <asm/msr.h>
#include <kernel/irq.h>

/* we havn't interrupts acknowledgment in microblaze architecture
 * and must receive interrupt number our self and then clear pending bit in
 * pending register
 */
void irq_handler(void) {
	unsigned int pending = 0;
	int cnt = 0;

	if (critical_inside(CRITICAL_IRQ_LOCK)) {
		cnt = 1;
	}

	cnt = cnt;

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		while (0 != (pending = mb_intc_get_pending())) {
			unsigned int irq_num;

			for (irq_num = 0; irq_num < IRQCTRL_IRQS_TOTAL; irq_num++) {
				if (pending & (1 << irq_num)) {
					//TODO we must clear whole pending register
					irqctrl_clear(irq_num);

					/*now we allow nested irq*/
					ipl_enable();

					irq_dispatch(irq_num);

					ipl_disable();
				}
			}
		}
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
