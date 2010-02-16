/**
 * @file
 * @brief TODO
 *
 * @date 16.02.2010
 * @author Eldar Abusalimov
 */

#include <errno.h>

#include <kernel/irq.h>

void irq_init(void) {
}

int irq_attach(irq_nr_t irq_nr, irq_handler_t handler, irq_flags_t flags,
		void *dev_id, const char *dev_name) {
	return -ENOSYS;
}

int irq_detach(irq_nr_t irq_nr, void *dev_id) {
	return -ENOSYS;
}

void irq_dispatch(interrupt_nr_t interrupt_nr) {
}
