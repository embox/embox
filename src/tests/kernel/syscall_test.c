/**
 * @file
 * @brief
 *
 * @date 06.11.2012
 * @author Anton Bulychev
 */

#include <embox/test.h>

#include <module/embox/arch/syscall.h>

EMBOX_TEST_SUITE("usermode");

TEST_CASE("syscall") {
	const char *s = "sys_write";
	test_assert_true(syscall_write(1, s, strlen(s)) == strlen(s));
}
