/**
 * @file
 *
 * @date 18.02.2015
 * @author Anton Bondarev
 */

#include <kernel/irq.h>

int irq_attach(unsigned int irq_nr, irq_handler_t handler,
		unsigned int flags, void *data, const char *dev_name) {
	return 0;
}

int irq_detach(unsigned int irq_nr, void *data) {
	return 0;
}
