/**
 * @file
 * @brief HAL IPL test.
 *
 * @date 06.03.2010
 * @author Eldar Abusalimov
 */

#include <embox/test.h>
#include <hal/ipl.h>
#include <test/misc.h>

#define TEST_IRQ_NR 10

EMBOX_TEST(run);

static int run(void) {
	ipl_t ipl;
	int ret;

	ipl = ipl_save();

	ret = (0 == test_misc_irq_force(TEST_IRQ_NR)) ? -1 : 0;

	ipl_restore(ipl);

	return ret;
}
