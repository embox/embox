/**
 * @file
 *
 * @date 15.03.11
 * @author Darya Dzendzik
 */

#include <embox/test.h>
#include <drivers/lcd.h>
#include <drivers/menu.h>

EMBOX_TEST(run_menu_test);

extern int menu_start(void);

static int run_menu_test(void) {
	display_clear_screen();
	menu_start();
	return 0;
}
