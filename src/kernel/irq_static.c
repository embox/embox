/**
 * @file
 * @brief
 *
 * @date Mar 12, 2014
 * @author: Anton Bondarev
 */
#include <kernel/irq.h>

int irq_attach(unsigned int irq_nr, irq_handler_t handler, unsigned int flags,
		void *dev_id, const char *dev_name) {
	return 0;
}

int irq_detach(unsigned int irq_nr, void *dev_id) {
	return 0;
}


void irq_dispatch(unsigned int irq_nr) {

}
