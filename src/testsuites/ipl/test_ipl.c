/**
 * @file
 * @brief HAL IPL test.
 *
 * @date 06.03.2010
 * @author Eldar Abusalimov
 */

#include <embox/test.h>
#include <hal/ipl.h>

EMBOX_TEST(run);

EMBOX_TEST_IMPORT(run_irq_force);

static int run(void) {
	ipl_t ipl;
	int ret;

	ipl = ipl_save();

	ret = (0 == run_irq_force()) ? -1 : 0;

	ipl_restore(ipl);

	return ret;
}
