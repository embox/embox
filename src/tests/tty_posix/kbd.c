/**
 * @file
 * @brief test of the keyboard
 *
 * @date 06.08.2011
 * @author Gerald Hoch
 */

#include <embox/test.h>
#include <embox/runlevel.h>
extern int tty_posix_kbd_diag_init(void);

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

	//don't make the test of the keyboard during the boot process....
	if (runlevel < 2)
		return 0;

	rc = tty_posix_kbd_diag_init();

	switch(rc) {
	case -1:
		test_fail("timeout");
	case -2:
		test_fail("search finished, no timeout, no key found");
	case -3:
		test_fail("wrong key found");
	}

	return rc;
}
