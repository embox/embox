/**
 * @brief Interrupt handler
 *
 * @date 15.12.22
 * @author Aleksey Zhmulin
 */
#include <assert.h>
#include <stddef.h>

#include <drivers/irqctrl.h>
#include <hal/cpu.h>
#include <kernel/cpu/bkl.h>
#include <kernel/critical.h>
#include <kernel/irq.h>

#include "exception.h"

#ifdef SMP
#include <aarch64/smp.h>
#endif

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

#ifdef SMP
	if (irq == AARCH64_SGI_STOP) {
		/* Served before critical_enter() takes the Big Kernel Lock: a core
		 * that parks holding it wedges every core that stops after it.
		 * Does not return. */
		aarch64_smp_stop_self(ctx->pc, ctx->lr, ctx->sp, ctx->psr);
	}
#endif

	/* An interrupt on a non-zero critical count takes the nested path in
	 * critical_enter(), which does not acquire the Big Kernel Lock because
	 * the count says this CPU holds it. Counted here, before the count is
	 * raised, so that a run can say how often that happened and whether the
	 * lock was really held. See bkl.c. */
	bkl_irq_total++;
	if (critical_count() & __CRITICAL_BKL_MASK) {
		bkl_irq_nested++;
		if (!bkl_owned()) {
			bkl_irq_unowned++;
		}
	}
	else {
		bkl_irq_zero++;
	}

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		/* Interrupts stay masked: a nested one would take a second exception
		 * frame and could switch context with the outer one still live */
		irq_dispatch(irq);
	}
	irqctrl_enable(irq);
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
