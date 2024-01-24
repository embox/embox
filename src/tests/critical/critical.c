/**
 * @file
 * @brief Kernel critical API test.
 *
 * @date 07.08.10
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <errno.h>
#include <hal/ipl.h>
#include <lib/libds/array.h>
#include <kernel/critical.h>

EMBOX_TEST_SUITE("critical API test");

TEST_SETUP_SUITE(suite_setup);
TEST_SETUP(case_setup);
TEST_TEARDOWN(case_teardown);

TEST_CASE("critical_inside should return false "
		"when called outside any critical section") {
	test_assert_false(critical_inside(CRITICAL_IRQ_LOCK));
	test_assert_false(critical_inside(CRITICAL_SCHED_LOCK));
}

TEST_CASE("critical_allows should return true "
		"when called outside any critical section") {
	test_assert_true(critical_allows(CRITICAL_IRQ_LOCK));
	test_assert_true(critical_allows(CRITICAL_SCHED_LOCK));
}

#if 0
static int run(void) {
	critical_enter(CRITICAL_SCHED_LOCK);

	test_assert_true(critical_inside(CRITICAL_SCHED_LOCK));
	test_assert_false(critical_inside(CRITICAL_IRQ_LOCK));

	test_assert_false(critical_allows(CRITICAL_SCHED_LOCK));

	critical_enter(CRITICAL_IRQ_LOCK);

	test_assert_true(critical_inside(CRITICAL_SCHED_LOCK));
	test_assert_true(critical_inside(CRITICAL_IRQ_LOCK));

	test_assert_false(critical_allows(CRITICAL_SCHED_LOCK));

	critical_leave(CRITICAL_IRQ_LOCK);
	critical_leave(CRITICAL_SCHED_LOCK);

	return 0;
}
#endif

static ipl_t ipl;

static int suite_setup(void) {
	/* Test must not be invoked within any critical section. */
	int inside_any = 0;
	inside_any |= critical_inside(CRITICAL_IRQ_LOCK);
	inside_any |= critical_inside(CRITICAL_SCHED_LOCK);
	return inside_any ? -EPERM : 0;
}

static int case_setup(void) {
	ipl = ipl_save();
	return 0;
}

static int case_teardown(void) {
	unsigned int critical, levels[] = {
		CRITICAL_IRQ_LOCK,
		CRITICAL_SCHED_LOCK,
	};

	array_foreach(critical, levels, ARRAY_SIZE(levels)) {
		while (critical_inside(critical)) {
			critical_leave(critical);
		}
	}

	ipl_restore(ipl);
	return 0;
}
