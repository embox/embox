/**
 * @file
 * @brief Kernel interrupt requests handling.
 *
 * @author Anton Bondarev
 *         - Initial implementation for SPARC
 *         - Extracting arch-independent code
 * @author Alexandr Batyukov, Alexey Fomin
 *         - Reviewing and rewriting some parts
 * @author Eldar Abusalimov
 *          - Rewriting from scratch:
 *          - Adapting for new HAL interface
 *          - Introducing locks and statistics accounting
 *          - Documentation
 */

#include <assert.h>
#include <errno.h>
#include <kernel/thread/sched.h>
#include <kernel/irq.h>
#include <kernel/softirq.h>
#include <hal/interrupt.h>
#include <hal/ipl.h>
#include <kernel/critical/api.h>
#include __impl_x(kernel/irq_critical.h)

struct irq_action {
	irq_handler_t handler;
	irq_flags_t flags;
	void *dev_id;
	const char *dev_name;
	unsigned int count_handled;
	unsigned int count_unhandled;
};

struct irq_entry {
	struct irq_action *action;
	unsigned int count;
};

/*
 * Temporal solution while preparing to introduce shared IRQs. This is just a
 * static place holder, in future this array will be replaced by dynamically
 * allocated linked lists.
 * TODO see above. -- Eldar
 */
static struct irq_action irq_actions[IRQ_NRS_TOTAL];

static struct irq_entry irq_table[IRQ_NRS_TOTAL];

void irq_init(void) {
	interrupt_init();
}

int irq_attach(irq_nr_t irq_nr, irq_handler_t handler, irq_flags_t flags,
		void *dev_id, const char *dev_name) {
	struct irq_action *action;
	ipl_t ipl;

	if (!irq_nr_valid(irq_nr) || NULL == handler) {
		return -EINVAL;
	}

	ipl = ipl_save();
	if (irq_table[irq_nr].action != NULL) {
		/* IRQ sharing is not supported for now... */
		ipl_restore(ipl);
		return -EBUSY;
	}

	/* Action allocation. */
	action = &irq_actions[irq_nr];

	action->handler = handler;
	action->flags = flags;
	action->dev_id = dev_id;
	action->dev_name = dev_name;
	action->count_handled = 0;
	action->count_unhandled = 0;

	irq_table[irq_nr].action = action;

	interrupt_enable(irq_nr);

	ipl_restore(ipl);
	return 0;
}

int irq_detach(irq_nr_t irq_nr, void *dev_id) {
	ipl_t ipl;

	if (!irq_nr_valid(irq_nr)) {
		return -EINVAL;
	}

	ipl = ipl_save();

	irq_table[irq_nr].action = NULL;

	ipl_restore(ipl);
	return 0;
}

static volatile unsigned int irq_nesting_count;

void irq_dispatch(interrupt_nr_t interrupt_nr) {
	irq_nr_t irq_nr = interrupt_nr;
	struct irq_action *action;
	irq_return_t irq_return;

	assert(interrupt_nr_valid(interrupt_nr));

	critical_enter(__CRITICAL_HARDIRQ);

	irq_table[irq_nr].count++;
	action = irq_table[irq_nr].action;
	if (action != NULL && action->handler != NULL) {
		irq_return = action->handler(irq_nr, action->dev_id);
		if (irq_return == IRQ_HANDLED) {
			action->count_handled++;
		} else {
			action->count_unhandled++;
		}
	}

	critical_leave(__CRITICAL_HARDIRQ);
}
