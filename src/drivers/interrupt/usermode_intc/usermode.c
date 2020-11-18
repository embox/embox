/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.03.2013
 */

#include <assert.h>
#include <stddef.h>
#include <kernel/host.h>
#include <kernel/critical.h>
#include <kernel/irq.h>
#include <drivers/irqctrl.h>

void irqctrl_enable(unsigned int interrupt_nr) {

}

void irqctrl_disable(unsigned int interrupt_nr) {

}

void irqctrl_clear(unsigned int interrupt_nr) {

}

void irqctrl_force(unsigned int interrupt_nr) {

        host_signal_send_self(interrupt_nr);
}

void irq_entry(int irq_nr) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{

		irq_dispatch(irq_nr);

	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

IRQCTRL_DEF(usermode_intc, NULL);
