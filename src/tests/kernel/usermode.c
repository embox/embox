/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#include <embox/test.h>

#include <module/embox/arch/usermode.h>

EMBOX_TEST_SUITE("usermode");

void *usermode_entry(void *arg) {
	__asm__ ("mov $1, %eax; int $0x80"); // call sys_exit
	return arg;
}

TEST_CASE("usermode") {
	call_in_usermode_if(1, usermode_entry, NULL);
}
