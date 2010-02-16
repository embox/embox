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
 *         - Rewriting from scratch:
 *          - Adapting for new HAL interface
 *          - Introducing locks and statistics accounting
 *          - Documentation
 */

#include <assert.h>
#include <errno.h>

#include <kernel/irq.h>
#include <kernel/softirq.h>
#include <hal/interrupt.h>
#include <hal/ipl.h>

#ifdef CONFIG_IRQ_ACCOUNT
#define action_count_handled_reset(action) \
	do { (action)->count_handled = 0; } while(0)
#define action_count_unhandled_reset(action) \
	do { (action)->count_unhandled = 0; } while(0)
#define action_count_handled_inc(action) \
	do { (action)->count_handled++; } while(0)
#define action_count_unhandled_inc(action) \
	do { (action)->count_unhandled++; } while(0)
#define entry_count_inc(entry) \
	do { (entry)->count++; } while(0)
#else
#define action_count_handled_reset(action) do { } while(0)
#define action_count_unhandled_reset(action) do { } while(0)
#define action_count_handled_inc(action) do { } while(0)
#define action_count_unhandled_inc(action) do { } while(0)
#define entry_count_inc(entry) do { } while(0)
#endif /* CONFIG_IRQ_ACCOUNT */

struct irq_action {
	irq_handler_t handler;
	irq_flags_t flags;
	void *dev_id;
	const char *dev_name;
#ifdef CONFIG_IRQ_ACCOUNT
	unsigned int count_handled;
	unsigned int count_unhandled;
#endif /* CONFIG_IRQ_ACCOUNT */
};

struct irq_entry {
	struct irq_action *action;
#ifdef CONFIG_IRQ_ACCOUNT
	unsigned int count;
#endif /* CONFIG_IRQ_ACCOUNT */
};

static struct irq_entry irq_table[IRQ_NRS_TOTAL];

/*
 * Temporal solution while preparing to introduce shared IRQs. This is just a
 * static place holder, in future this array will be replaced by dynamically
 * allocated linked lists.
 * TODO see above. -- Eldar
 */
static struct irq_action irq_actions[IRQ_NRS_TOTAL];

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
	action_count_handled_reset(action);
	action_count_unhandled_reset(action);

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

#if 0
int irq_info(irq_nr_t irq_nr, struct irq_info *info) {
	struct irq_action *action;
	ipl_t flags;

	if (!irq_nr_valid(irq_nr) || NULL == info) {
		return -EINVAL;
	}

	local_irq_save(flags);

	action = irq_table[irq_nr]->action;
	if (NULL == action) {
		local_irq_restore(flags);
		return -ENOENT;
	}

	local_irq_restore(flags);

	return 0;
}
#endif

static volatile unsigned int irq_nesting_count;

#ifdef CONFIG_SOFTIRQ

static void irq_enter(void) {
	ipl_t ipl;

	ipl = ipl_save();
	irq_nesting_count++;
	ipl_restore(ipl);
}

static void irq_leave(void) {
	ipl_t ipl;
	unsigned int count;

	ipl = ipl_save();
	count = --irq_nesting_count;
	ipl_restore(ipl);

	if (0 == count) {
		/* Leaving the interrupt context. */
		softirq_dispatch();
	}
}

#else
#define irq_enter() do { } while(0)
#define irq_leave() do { } while(0)
#endif

void irq_dispatch(interrupt_nr_t interrupt_nr) {
	irq_nr_t irq_nr = interrupt_nr;
	struct irq_action *action;
	irq_return_t irq_return;

	assert(interrupt_nr_valid(interrupt_nr));

	irq_enter();

	entry_count_inc(&irq_table[irq_nr]);
	action = irq_table[irq_nr].action;
	if (action != NULL && action->handler != NULL) {
		irq_return = action->handler(irq_nr, action->dev_id);
		if (irq_return == IRQ_HANDLED) {
			action_count_handled_inc(action);
		} else {
			action_count_unhandled_inc(action);
		}
	}

	irq_leave();
}
