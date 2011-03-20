/**
 * @file
 * @brief This file is derived from Embox test template.
 */

#include <embox/test.h>

#include <kernel/critical.h>

EMBOX_TEST(run);

static int test_outside_critical(void) {
	test_assert(!critical_inside_preempt());
	test_assert(!critical_inside_softirq());
	test_assert(!critical_inside_hardirq());

	test_assert(critical_allows_preempt());
	test_assert(critical_allows_softirq());

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
//	if (!critical_allows_preempt()) {
//		return -EBUSY;
//	}

	if (0 != (result = test_outside_critical())) {
		return result;
	}

	critical_enter_preempt();

	test_assert(critical_inside_preempt());
	test_assert(!critical_inside_softirq());
	test_assert(!critical_inside_hardirq());

	test_assert(!critical_allows_preempt());
	test_assert(critical_allows_softirq());

	critical_enter_softirq();

	test_assert(critical_inside_preempt());
	test_assert(critical_inside_softirq());
	test_assert(!critical_inside_hardirq());

	test_assert(!critical_allows_preempt());
	test_assert(!critical_allows_softirq());

	critical_enter_hardirq();

	test_assert(critical_inside_preempt());
	test_assert(critical_inside_softirq());
	test_assert(critical_inside_hardirq());

	test_assert(!critical_allows_preempt());
	test_assert(!critical_allows_softirq());

	critical_leave_hardirq();
	critical_leave_softirq();
	critical_leave_preempt();

	if (0 != (result = test_outside_critical())) {
		return result;
	}

//	critical_enter_softirq();

	return test_outside_critical();
}
