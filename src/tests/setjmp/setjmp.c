/**
 * @file
 * @brief Test for setjmp facility.
 *
 * @date 06.03.10
 * @author Eldar Abusalimov
 */

#include <setjmp.h>
#include <embox/test.h>

#define TEST_JUMP_VALUE 0xFADE

EMBOX_TEST(run);

static int run(void) {
	jmp_buf jb;

	switch (setjmp(jb)) {
	case 0:
		longjmp(jb, TEST_JUMP_VALUE);
		TRACE("longjmp returned\n");
		return -1;
	case TEST_JUMP_VALUE:
		return 0;
	default:
		TRACE("setjmp unexpected return value\n");
		return -1;
	}
}
