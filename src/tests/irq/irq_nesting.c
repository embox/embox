/**
 * @file
 *
 * @date 17.03.10
 * @author Eldar Abusalimov
 */

#include <types.h>
#include <string.h>

#include <embox/test.h>
#include <kernel/irq.h>
#include <hal/interrupt.h>

#define TEST_OUTER_IRQ_NR 10
#define TEST_INNER_IRQ_NR (TEST_OUTER_IRQ_NR + 1)

EMBOX_TEST(run);

volatile static bool outer_irq, inner_irq, irq_nested;

static counter = 100;

static irq_return_t test_isr(irq_nr_t irq_nr, void *dev_id) {
	if (!outer_irq) {
		outer_irq = true;
		interrupt_force(TEST_INNER_IRQ_NR);
		while (counter--); /* some processors needs few cycle to reassert irq */
		if (inner_irq) {
			irq_nested = true;
		} else {
			TRACE("no inner irq received being inside outer irq handler\n");
		}
	} else if (!inner_irq) {
		inner_irq = true;
	} else {
		TRACE("unexpected irq nr %d\n", irq_nr);
	}
	return IRQ_HANDLED;
}

static int run(void) {
	int error;

	outer_irq = false;
	inner_irq = false;
	irq_nested = false;

	if (0 != (error = irq_attach(TEST_OUTER_IRQ_NR, test_isr, 0x0, NULL,
			"test_irq_first")) || 0 != (error = irq_attach(TEST_INNER_IRQ_NR,
			test_isr, 0x0, NULL, "test_irq_second"))) {
		TRACE("irq_attach failed: %s\n", strerror(-error));
		return -1;
	}

	interrupt_force(TEST_OUTER_IRQ_NR);

	irq_detach(TEST_OUTER_IRQ_NR, NULL);
	irq_detach(TEST_INNER_IRQ_NR, NULL);

	return outer_irq && inner_irq && irq_nested ? 0 : -1;
}
