/**
 * @file
 * @brief TODO
 *
 * @date 24.01.2010
 * @author Eldar Abusalimov
 */

#ifndef LINUX_INTERRUPT_H_
#define LINUX_INTERRUPT_H_

#include <kernel/irq.h>
#include <kernel/softirq.h>

/**
 * The value returned from request_irq to the requesting function is either 0
 * to indicate success or a negative error code, as usual.
 * It's not uncommon for the function to return -EBUSY to signal
 * that another driver is already using the requested interrupt line.
 * The arguments to the functions are as follows:
 * @param irq This is the interrupt number being requested.
 * @param handler The pointer to the handling function being installed.
 *  We'll discuss the arguments to this function later in this chapter.
 * @param flags As you might expect, a bit mask of options (described later)
 *  related to interrupt management.
 * @param dev_name The string passed to request_irq is used in /proc/interrupts
 *  to show the owner of the interrupt (see the next section).
 * @param dev_id This pointer is used for shared interrupt lines.
 *  It is a unique identifier that is used when the interrupt line is freed
 *  and that may also be used by the driver to point to its own private data
 *  area (to identify which device is interrupting). When no sharing is in force,
 *  dev_id can be set to NULL, but it a good idea anyway to use this item to point
 *  to the device structure.
 */
static inline int request_irq(unsigned int irq, irq_handler_t handler,
		unsigned long flags, const char *dev_name, void *dev_id) {
	return irq_attach(irq, handler, flags, dev_id, dev_name);
}

static inline void free_irq(unsigned int irq, void *dev_id) {
	irq_detach(irq, dev_id);
}

enum {
	HI_SOFTIRQ = 0,
	TIMER_SOFTIRQ,
	NET_TX_SOFTIRQ,
	NET_RX_SOFTIRQ,
	BLOCK_SOFTIRQ,
	TASKLET_SOFTIRQ,
	SCHED_SOFTIRQ
};

struct softirq_action {
	void (*action)(struct softirq_action *);
	void *data;
};

extern void open_softirq(int nr, void(*action)(struct softirq_action *),
		void *data);

#define raise_softirq(nr) softirq_raise(nr)

typedef irq_return_t irqreturn_t;

#endif /* LINUX_INTERRUPT_H_ */
