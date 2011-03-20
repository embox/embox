/**
 * @file
 * @brief This file is derived from Embox test template.
 */

#include <embox/test.h>

#include <kernel/critical.h>

EMBOX_TEST(run);

static int test_outside_critical(void) {
	test_assert_false(critical_inside(CRITICAL_PREEMPT));
	test_assert_false(critical_inside(CRITICAL_SOFTIRQ));
	test_assert_false(critical_inside(CRITICAL_HARDIRQ));

	test_assert_true(critical_allows(CRITICAL_PREEMPT));
	test_assert_true(critical_allows(CRITICAL_SOFTIRQ));

	return 0;
}

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
static int run(void) {
	int result = 0;

	/* Test must not be invoked within critical section. */
//	if (!critical_allows(CRITICAL_PREEMPT)) {
//		return -EBUSY;
//	}

	if (0 != (result = test_outside_critical())) {
		return result;
	}

	critical_enter(CRITICAL_PREEMPT);

	test_assert_true(critical_inside(CRITICAL_PREEMPT));
	test_assert_false(critical_inside(CRITICAL_SOFTIRQ));
	test_assert_false(critical_inside(CRITICAL_HARDIRQ));

	test_assert_false(critical_allows(CRITICAL_PREEMPT));
	test_assert_true(critical_allows(CRITICAL_SOFTIRQ));

	critical_enter(CRITICAL_SOFTIRQ);

	test_assert_true(critical_inside(CRITICAL_PREEMPT));
	test_assert_true(critical_inside(CRITICAL_SOFTIRQ));
	test_assert_false(critical_inside(CRITICAL_HARDIRQ));

	test_assert_false(critical_allows(CRITICAL_PREEMPT));
	test_assert_false(critical_allows(CRITICAL_SOFTIRQ));

	critical_enter(CRITICAL_HARDIRQ);

	test_assert_true(critical_inside(CRITICAL_PREEMPT));
	test_assert_true(critical_inside(CRITICAL_SOFTIRQ));
	test_assert_true(critical_inside(CRITICAL_HARDIRQ));

	test_assert_false(critical_allows(CRITICAL_PREEMPT));
	test_assert_false(critical_allows(CRITICAL_SOFTIRQ));

	critical_leave(CRITICAL_HARDIRQ);
	critical_leave(CRITICAL_SOFTIRQ);
	critical_leave(CRITICAL_PREEMPT);

	if (0 != (result = test_outside_critical())) {
		return result;
	}

//	critical_enter(CRITICAL_SOFTIRQ);

	return test_outside_critical();
}
