/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#include <embox/test.h>

#include <module/embox/arch/usermode.h>
#include <module/embox/arch/syscall.h>

EMBOX_TEST_SUITE("usermode");

void *usermode_entry(void *arg) {
	const char *s = "syscall inside usermode";
	test_assert_true(syscall_write(1, s, strlen(s)) == strlen(s));
	return arg;
}

TEST_CASE("usermode") {
	usermode_call_and_switch_if(1, usermode_entry, NULL);
}
