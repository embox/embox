/**
 * @file
 * @brief test of screen
 *
 * @date 06.08.2011
 * @author Gerald Hoch
 */

#include <embox/test.h>
#include <embox/runlevel.h>
extern int ttyo_diag_init(void);

EMBOX_TEST(run);

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
static int run(void) {
	int rc;
	runlevel_nr_t runlevel = runlevel_get_entered();

	//don't make the test of the screen during the boot process....
	if (runlevel < 2)
		return 0;

	rc = ttyo_diag_init();
	if (rc == -1)
		test_fail("timeout or wrong key");

	return rc;
}
