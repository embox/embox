/**
 * @file
 *
 * @date 05.04.2017
 * @author Anton Bondarev
 */

#include <embox/test.h>

EMBOX_TEST_SUITE("armv7 debug tests");

TEST_CASE("bptk instraction code must raise undef exception") {
	__asm__ __volatile__ ("bkpt 0x1234;");
}
