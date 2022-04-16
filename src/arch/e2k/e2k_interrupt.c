/**
 * @file
 * @brief
 *
 * @date Mar 13, 2018
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <assert.h>

#include <kernel/printk.h>

#include <kernel/irq.h>
#include <asm/ptrace.h>
#include <asm/trap_def.h>

#include <drivers/interrupt/lapic/regs.h>

void e2k_interrupt_handler(void) {
	int irq;

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	irq = lapic_get_irq();

	log_debug(">> handle irq %d\n", irq);

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		irq_dispatch(irq);

		irqctrl_eoi(irq);
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

