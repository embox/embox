/**
 * @file
 * @brief Kernel interrupt requests handling.
 *
 * @date 01.12.2008
 * @author Anton Bondarev
 *           - Initial contribution for SPARC
 *           - Extracting arch-independent code
 * @author Alexandr Batyukov, Alexey Fomin
 *           - Reviewing and rewriting some parts
 * @author Eldar Abusalimov
 *           - Rewriting from scratch:
 *               - Adapting for new HAL interface
 *               - Introducing locks and statistics accounting
 *               - Documentation
 *           - Using object allocator for internal structures
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include <kernel/irq.h>
#include <kernel/irq_lock.h>
#include <kernel/critical.h>
#include <hal/interrupt.h>
#include <hal/ipl.h>
#include <mem/objalloc.h>
#include <profiler/tracing/trace.h>

struct irq_action {
	irq_handler_t handler;
	void *dev_id;
};

OBJALLOC_DEF(irq_actions, struct irq_action, IRQ_NRS_TOTAL);

static struct irq_action *irq_table[IRQ_NRS_TOTAL];

int irq_attach(irq_nr_t irq_nr, irq_handler_t handler, unsigned int flags,
		void *dev_id, const char *dev_name) {
	struct irq_action *action;
	int ret = ENOERR;

	if (!irq_nr_valid(irq_nr) || !handler) {
		return -EINVAL;
	}

	irq_lock();

	trace_point("irq attach");

	if (irq_table[irq_nr]) {
		/* IRQ sharing is not supported for now... */
		ret = -EBUSY;
		goto out_unlock;
	}

	/* Action allocation. */
	if (!(action = objalloc(&irq_actions))) {
		ret = -ENOMEM;
		goto out_unlock;
	}

	action->handler = handler;
	action->dev_id = dev_id;

	irq_table[irq_nr] = action;

	interrupt_enable(irq_nr);

	out_unlock: irq_unlock();
	return ret;
}

int irq_detach(irq_nr_t irq_nr, void *dev_id) {
	struct irq_action *action;
	int ret = ENOERR;

	if (!irq_nr_valid(irq_nr)) {
		return -EINVAL;
	}

	irq_lock();

	trace_point("irq detach");

	if (!(action = irq_table[irq_nr]) || action->dev_id != dev_id) {
		ret = -ENOENT;
		goto out_unlock;
	}

	objfree(&irq_actions, action);
	irq_table[irq_nr] = NULL;

	out_unlock: irq_unlock();
	return ret;
}

void irq_dispatch(interrupt_nr_t interrupt_nr) {
	irq_nr_t irq_nr = interrupt_nr;
	struct irq_action *action;
	irq_handler_t handler = NULL;
	void *dev_id = NULL;
	ipl_t ipl;

	assert(interrupt_nr_valid(interrupt_nr));

	trace_point("interrupt");

	ipl = ipl_save();
	{
		if ((action = irq_table[irq_nr])) {
			handler = action->handler;
			dev_id = action->dev_id;
		}
	}
	ipl_restore(ipl);

	if (!action) {
		return;
	}

	assert(handler != NULL);
	handler(irq_nr, dev_id);
}

