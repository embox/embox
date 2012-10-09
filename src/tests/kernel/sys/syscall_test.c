/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#include <embox/test.h>

EMBOX_TEST_SUITE("basic syscall tests");

TEST_CASE("syscall") {
	__asm__ ("int $0x80");
}
