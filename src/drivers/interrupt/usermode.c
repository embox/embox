/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.03.2013
 */

#include <errno.h>
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

static int irq_queue(void) {
	struct emvisor_msghdr msg;
	int ret;

	if (0 >= (ret = emvisor_recvmsg(UV_PRDDOWNSTRM, &msg))) {
		return ret;
	}

	if (msg.type <= EMVISOR_IRQ) {
		return -ENOENT;
	}

	ipl_enable();
	{

		irq_dispatch(msg.type - EMVISOR_IRQ);

	}
	ipl_disable();

	return ret;
}

void irq_entry(void) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{

		while (0 < irq_queue());

		emvisor_send(UV_PWRUPSTRM, EMVISOR_EOF_IRQ, NULL, 0);

	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
