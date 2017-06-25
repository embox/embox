/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.02.2013
 */

#include <unistd.h>

#include <embox/test.h>

#define DEFL_UID 0
#define TEST_UID 1
#define TEST_UID2 2

EMBOX_TEST_SUITE("User id's");

TEST_TEARDOWN(teardown_test);

TEST_CASE("setuid should be compatible with POSIX") {
	test_assert_zero(setuid(TEST_UID));

	test_assert_equal(getuid(), TEST_UID);
	test_assert_equal(geteuid(), TEST_UID);

	test_assert_equal(setuid(TEST_UID2), -1);

	test_assert_equal(getuid(), TEST_UID);
	test_assert_equal(geteuid(), TEST_UID);

}

TEST_CASE("seteuid should be compatible with POSIX") {
	test_assert_zero(seteuid(TEST_UID));

	test_assert_equal(getuid(), DEFL_UID);
	test_assert_equal(geteuid(), TEST_UID);

	test_assert_equal(setuid(TEST_UID2), -1);

	test_assert_equal(getuid(), DEFL_UID);
	test_assert_equal(geteuid(), TEST_UID);

}

#include <kernel/task/resource/u_area.h>
static int teardown_test(void) {
        memset(task_self_resource_u_area(), 0, sizeof(struct task_u_area));
	return 0;
}
