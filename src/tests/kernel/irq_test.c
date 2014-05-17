/**
 * @file
 *
 * @date 17.03.10
 * @author Eldar Abusalimov
 */

#include <embox/test.h>
#include <kernel/irq.h>
#include <drivers/irqctrl.h>

EMBOX_TEST_SUITE("IRQ tests");

#define TEST_IRQ_NR OPTION_GET(NUMBER,irq_nr)

#define TEST_OUTER_IRQ_NR OPTION_GET(NUMBER,outer_irq_nr)
#define TEST_INNER_IRQ_NR OPTION_GET(NUMBER,inner_irq_nr)

#if OPTION_GET(BOOLEAN,forced_irq_clear)
#define test_irq_clear_irq(x) irqctrl_clear(x)
#else
#define test_irq_clear_irq(x)
#endif

static irq_return_t test_isr(unsigned int irq_nr, void *dev_id) {
	test_emit((int) dev_id);
	test_irq_clear_irq(irq_nr);
	return IRQ_HANDLED;
}

TEST_CASE("An IRQ handler must run immediately after forcing the interrupt") {
	test_emit('{');

	test_assert_zero(
			irq_attach(TEST_IRQ_NR, test_isr, 0, (void *) '0', "test_irq"));

	test_emit('[');
	irqctrl_force(TEST_IRQ_NR);
	test_emit(']');

	test_assert_zero(irq_detach(TEST_IRQ_NR, (void *) '0'));

	test_emit('}');

	test_assert_emitted("{[0]}");
}

#define FIBONACCI_NR 16

static int fib(int k) {
	if (k < 2) {
		return k;
	} else {
		return fib(k - 1) + fib(k - 2);
	}
}

static irq_return_t test_fib_isr(unsigned int irq_nr, void *dev_id) {
	*(int *) dev_id = fib(*(int *) dev_id);
	test_irq_clear_irq(irq_nr);
	return IRQ_HANDLED;
}

TEST_CASE("Deep IRQ handler regression "
		"(recursively calculating Fibonacci number inside IRQ handler)") {
	int result = FIBONACCI_NR;

	test_assert_zero(
			irq_attach(TEST_IRQ_NR, test_fib_isr, 0, &result, "test_irq"));
	irqctrl_force(TEST_IRQ_NR);
	test_assert_zero(irq_detach(TEST_IRQ_NR, &result));

	test_assert_equal(result, fib(FIBONACCI_NR));
}

static irq_return_t test_nesting_isr(unsigned int irq_nr, void *dev_id) {
	test_emit((int) dev_id);
	if (irq_nr == TEST_OUTER_IRQ_NR) {
		test_emit('(');
		irqctrl_force(TEST_INNER_IRQ_NR);
		test_emit(')');
	}
	test_emit('x');
	test_irq_clear_irq(irq_nr);
	return IRQ_HANDLED;
}

TEST_CASE("An IRQ with higher priority should preempt a handler "
		"of a lower one") {
	test_emit('{');

	test_assert_zero(
			irq_attach(TEST_OUTER_IRQ_NR, test_nesting_isr, 0,
					(void *) '0', "test_irq_outer"));
	test_assert_zero(
			irq_attach(TEST_INNER_IRQ_NR, test_nesting_isr, 0,
					(void *) '1', "test_irq_inner"));

	test_emit('[');
	irqctrl_force(TEST_OUTER_IRQ_NR);
	test_emit(']');

	test_assert_zero(irq_detach(TEST_OUTER_IRQ_NR, (void *) '0'));
	test_assert_zero(irq_detach(TEST_INNER_IRQ_NR, (void *) '1'));

	test_emit('}');

	test_assert_emitted("{[0(1x)x]}");
}
