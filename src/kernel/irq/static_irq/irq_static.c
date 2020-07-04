/**
 * @file
 * @brief
 *
 * @date Mar 12, 2014
 * @author: Anton Bondarev
 */

#include <assert.h>
#include <kernel/irq.h>

extern char __static_irq_table_start;
extern char __static_irq_table_end;

#define STATIC_IRQ_TABLE_SIZE \
	(((int) &__static_irq_table_end - (int) &__static_irq_table_start) / 4)

int irq_attach(unsigned int irq_nr, irq_handler_t handler, unsigned int flags,
		void *dev_id, const char *dev_name) {
	assert(irq_nr <= STATIC_IRQ_TABLE_SIZE);
	assertf(((void **) &__static_irq_table_start)[irq_nr] != NULL,
			"IRQ(%d) handler is not assigned with STATIC_IRQ_ATTACH\n", irq_nr);

	irqctrl_enable(irq_nr);
	return 0;
}

int irq_detach(unsigned int irq_nr, void *dev_id) {
	irqctrl_disable(irq_nr);
	return 0;
}

void irq_dispatch(unsigned int irq_nr) {

}
