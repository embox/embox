/**
 * @file
 *
 * @date 02.11.2009
 * @author Alexey Fomin
 * @author Eldar Abusalimov
 */

#include "common.h"

#include <types.h>
#include <string.h>

#include <embox/test.h>
#include <kernel/irq.h>
#include <hal/interrupt.h>

#define TEST_IRQ_NR 10

EMBOX_TEST(run);
EMBOX_TEST_EXPORT(run_irq_force);

volatile static bool irq_first, irq_second;

static irq_return_t test_isr(irq_nr_t irq_nr, void *dev_id) {
	if (!irq_first) {
		TRACE(" first(%d) { ", irq_nr);
		irq_first = true;
		interrupt_force(TEST_IRQ_NR + 1);
		TRACE(" } first ");
	} else if (!irq_second) {
		TRACE(" second(%d) ", irq_nr);
		irq_second = true;
	} else {
		TRACE(" third(%d) ", irq_nr);
	}
	return IRQ_HANDLED;
}

static int run(void) {
	int error;

	irq_first = false;
	irq_second = false;

	if (0 != (error = irq_attach(TEST_IRQ_NR, test_isr, 0x0, NULL,
			"test_irq_first")) || 0 != (error = irq_attach(TEST_IRQ_NR + 1,
			test_isr, 0x0, NULL, "test_irq_second"))) {
		TRACE("irq_attach failed: %s\n", strerror(error));
		return -1;
	}

	interrupt_force(TEST_IRQ_NR);

	irq_detach(TEST_IRQ_NR, NULL);
	irq_detach(TEST_IRQ_NR + 1, NULL);

	return irq_first && irq_second ? 0 : -1;
}
