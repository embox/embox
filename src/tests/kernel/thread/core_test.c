/**
 * @file
 * @brief TODO documentation for core_test.c -- Alina Kramar
 *
 * @date 09.04.2011
 * @author Alina Kramar
 */

#include <embox/test.h>

#include <kernel/thread/api.h>

EMBOX_TEST_SUITE("test for thread API");

static void *foo_run(void *arg) {
	return arg;
}

TEST_CASE("thread_join should retrieve the result of thread execution") {
	struct thread *foo;
	int ret;

	test_assert_zero(thread_create(&foo, 0, foo_run, 42));
	test_assert_zero(thread_join(foo, &ret));
	test_assert_equal(ret, 42);

}
