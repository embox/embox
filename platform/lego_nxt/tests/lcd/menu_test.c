/**
 * @file
 *
 * @date 15.03.2011
 * @author Darya Dzendzik
 */

#include <embox/test.h>
#include <drivers/lcd.h>
#include <unistd.h>
#include <types.h>
#include <drivers/menu.h>
#include <drivers/nxt_buttons.h>

EMBOX_TEST(run_menu_test);

extern int menu_start(void);

static int run_menu_test(void){
	display_clear_screen();
	menu_start();
	return 0;
}
