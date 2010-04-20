/**
 * @file
 *
 * @date 02.11.2009
 * @author Eldar Abusalimov
 */

#include <stdbool.h>
#include <string.h>

#include <embox/test.h>
#include <kernel/irq.h>
#include <hal/interrupt.h>

#define TEST_IRQ_NR 10

EMBOX_TEST(run);
EMBOX_TEST_IMPORT(run_recursion);

volatile static bool irq_happened;

static irqreturn_t test_isr(irq_nr_t irq_nr, void *dev_id) {
	run_recursion();
	irq_happened = true;
	return IRQ_HANDLED;
}

static int run(void) {
	int error;

	irq_happened = false;

	if (0 != (error = irq_attach(TEST_IRQ_NR, test_isr, 0x0, NULL,
			"test_irq_force"))) {
		TRACE("irq_attach failed: %s\n", strerror(-error));
		return -1;
	}

	interrupt_force(TEST_IRQ_NR);

	irq_detach(TEST_IRQ_NR, NULL);

	return irq_happened ? 0 : -1;
}
