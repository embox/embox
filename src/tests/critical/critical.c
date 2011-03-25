/**
 * @file
 * @brief Kernel critical API test.
 */

#include <embox/test.h>

#include <errno.h>
#include <hal/ipl.h>
#include <util/array.h>
#include <kernel/critical/api.h>

EMBOX_TEST_SUITE("critical API test");

TEST_SETUP_SUITE(suite_setup);
TEST_SETUP(case_setup);
TEST_TEARDOWN(case_teardown);

TEST_CASE("critical_inside should return false "
		"when called outside any critical section") {
	test_assert_false(critical_inside(CRITICAL_HARDIRQ));
	test_assert_false(critical_inside(CRITICAL_SOFTIRQ));
	test_assert_false(critical_inside(CRITICAL_PREEMPT));
}

TEST_CASE("critical_allows should return true "
		"when called outside any critical section") {
	test_assert_true(critical_allows(CRITICAL_HARDIRQ));
	test_assert_true(critical_allows(CRITICAL_SOFTIRQ));
	test_assert_true(critical_allows(CRITICAL_PREEMPT));
}

#if 0
static int run(void) {
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

	return 0;
}
#endif

static ipl_t ipl;

static int suite_setup(void) {
	/* Test must not be invoked within any critical section. */
	int inside_any = 0;
	inside_any |= critical_inside(CRITICAL_HARDIRQ);
	inside_any |= critical_inside(CRITICAL_SOFTIRQ);
	inside_any |= critical_inside(CRITICAL_PREEMPT);
	return inside_any ? -EPERM : 0;
}

static int case_setup(void) {
	ipl = ipl_save();
	return 0;
}

static int case_teardown(void) {
	critical_t critical, levels[] = { CRITICAL_HARDIRQ, CRITICAL_SOFTIRQ,
			CRITICAL_PREEMPT };

	array_static_foreach(critical, levels) {
		while (critical_inside(critical)) {
			critical_leave(critical);
		}
	}

	ipl_restore(ipl);
	return 0;
}

