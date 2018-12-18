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

extern void *trap_table_start;

#define TRAP_TABLE_SIZE \
	((int) &__static_irq_table_start - (int) &trap_table_start)

#define STATIC_IRQ_TABLE_SIZE \
	((int) &__static_irq_table_end - (int) &__static_irq_table_start)

#define MAX_IRQ_NUMBER \
	((STATIC_IRQ_TABLE_SIZE + TRAP_TABLE_SIZE - 4) / 4)

int irq_attach(unsigned int irq_nr, irq_handler_t handler, unsigned int flags,
		void *dev_id, const char *dev_name) {
	/* Probably, it can be a different value for different Cortex-M */
	assert(TRAP_TABLE_SIZE == (11 * 4));
	assert(irq_nr <= MAX_IRQ_NUMBER);

	irqctrl_enable(irq_nr);
	return 0;
}

int irq_detach(unsigned int irq_nr, void *dev_id) {
	irqctrl_disable(irq_nr);
	return 0;
}

void irq_dispatch(unsigned int irq_nr) {

}
