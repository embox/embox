/**
 * @file
 * @brief HAL IPL test.
 *
 * @date 06.03.10
 * @author Eldar Abusalimov
 */

#include <embox/test.h>
#include <kernel/irq.h>
#include <hal/ipl.h>
#include <drivers/irqctrl.h>

EMBOX_TEST_SUITE("Interrupt priority level tests");

#define TEST_IRQ_NR 10

static irq_return_t test_isr(unsigned int irq_nr, void *dev_id) {
	test_emit((int) dev_id);
	return IRQ_HANDLED;
}

TEST_CASE("An interrupt forced after a call to ipl_save() must not run until "
		"the corresponding ipl_restore() is called") {
	ipl_t ipl;

	test_emit('{');

	test_assert_zero(
			irq_attach(TEST_IRQ_NR, test_isr, 0, (void *) '0', "test_irq"));

	ipl = ipl_save();

	test_emit('[');
	irqctrl_force(TEST_IRQ_NR);
	test_emit(']');

	ipl_restore(ipl);

	test_assert_zero(irq_detach(TEST_IRQ_NR, (void *) '0'));

	test_emit('}');

	test_assert_emitted("{[]0}");
}
