/**
 * @file
 * @brief TODO
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <test/misc.h>
#include <kernel/irq.h>
#include <hal/interrupt.h>

volatile static bool irq_happened;
volatile static int cb_retval;

int test_misc_irq_force(irq_nr_t nr) {
	return test_misc_irq_force_callback(nr, NULL);
}

static irq_return_t test_isr(irq_nr_t irq_nr, void *dev_id) {
	int(*cb)(void) = (int(*)(void)) (int) dev_id;

	irq_happened = true;
	cb_retval = cb ? cb() : 0;

	return IRQ_HANDLED;
}

int test_misc_irq_force_callback(irq_nr_t nr, int (*cb)(void)) {
	int error;

	irq_happened = false;
	cb_retval = 0;

	if (0 != (error = irq_attach(nr, test_isr, 0x0, (void *) (int) cb,
			"test_misc_irq_force"))) {
		return error;
	}

	interrupt_force(nr);

	irq_detach(nr, (void *) (int) cb);

	return irq_happened ? cb_retval : -EIO;
}
