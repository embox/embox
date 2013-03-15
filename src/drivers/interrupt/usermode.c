/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.03.2013
 */

#include <stddef.h>
#include <assert.h>
#include <kernel/host.h>
#include <kernel/critical.h>
#include <hal/ipl.h>
#include <kernel/irq.h>
#include <drivers/irqctrl.h>
#include <kernel/umirq.h>

#include <prom/prom_printf.h>

void irqctrl_enable(unsigned int interrupt_nr) {

}

void irqctrl_disable(unsigned int interrupt_nr) {

}

void irqctrl_clear(unsigned int interrupt_nr) {

}

void irqctrl_force(unsigned int interrupt_nr) {

	emvisor_sendirq(host_getpid(), 1, UV_PWRDOWNSTRM,
			EMVISOR_IRQ + interrupt_nr, NULL, 0);
}

void irq_entry(int irq) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		ipl_enable();

		/*prom_printf("received %d\n", irq);*/

		irq_dispatch(irq);

		ipl_disable();
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
