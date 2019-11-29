/**
 * @file
 * @brief Test for setjmp facility.
 *
 * @date 06.03.10
 * @author Eldar Abusalimov
 */

#include <setjmp.h>
#include <stdio.h>
#include <embox/test.h>
//#include <kernel/printk.h>
//#include <asm/regs.h>

#define TEST_JUMP_VALUE 0xFADE

EMBOX_TEST_SUITE("stdlib/setjmp test");

TEST_CASE("Save context with 'setjmp' function than restore with 'longjmp' call") {
	jmp_buf jb;
	int *a = NULL;
	*a = 5;
	switch (setjmp(jb)) {
	case 0:
		longjmp(jb, TEST_JUMP_VALUE);
		test_fail("We passed longjmp call");
		break;

	case TEST_JUMP_VALUE:
//		printk("mstatus: %lx\n", (read_csr(mstatus)));
		test_assert(true);
		break;

	default:
		test_fail("Setjmp return strange value");
		break;
	}
}
