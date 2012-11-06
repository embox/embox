/**
 * @file
 * @brief
 *
 * @date 06.11.2012
 * @author Anton Bulychev
 */

#include <embox/test.h>

#include <kernel/syscall_caller.h>

EMBOX_TEST_SUITE("syscall test");

TEST_CASE("calling sys_write") {
	const char *s = "sys_write";
	test_assert_true(syscall_write(1, s, strlen(s)) == strlen(s));
}
