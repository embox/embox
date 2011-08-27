/**
 * @file
 *
 * @date 17.03.10
 * @author Eldar Abusalimov
 */

#include <embox/test.h>
#include <kernel/irq.h>
#include <hal/interrupt.h>

#define TEST_OUTER_IRQ_NR 10
#define TEST_INNER_IRQ_NR (TEST_OUTER_IRQ_NR + 1)

EMBOX_TEST_SUITE("IRQ tests");

static irq_return_t test_isr(irq_nr_t irq_nr, void *dev_id) {
	test_emit((int) dev_id);
	if (irq_nr == TEST_OUTER_IRQ_NR) {
		test_emit('(');
		interrupt_force(TEST_INNER_IRQ_NR);
		test_emit(')');
	}
	test_emit('x');
	return IRQ_HANDLED;
}

TEST_CASE("TODO") {
	test_emit('a');

	test_assert_zero(irq_attach(TEST_OUTER_IRQ_NR, test_isr, 0,
			(void *) '0', "test_irq_outer"));
	test_assert_zero(irq_attach(TEST_INNER_IRQ_NR, test_isr, 0,
			(void *) '1', "test_irq_inner"));

	test_emit('{');
	interrupt_force(TEST_OUTER_IRQ_NR);
	test_emit('}');

	test_assert_zero(irq_detach(TEST_OUTER_IRQ_NR, (void *) '0'));
	test_assert_zero(irq_detach(TEST_INNER_IRQ_NR, (void *) '1'));

	test_emit('z');

	test_assert_emitted("a{0(1x)x}z");
}
