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

extern uint8_t change_pointer(uint8_t y, uint8_t current_test, int buts);
extern uint8_t move_list(uint8_t current_test, int buts);

static int run_menu_test(void){
	uint8_t y, current_test, max_cur_test, min_cur_test, current_test_new;
	int buts = nxt_buttons_was_pressed();
	int i = 100;
	TRACE ("CCCccc EEEeee\n");
	current_test = 0;
	min_cur_test = 0;
	max_cur_test = 7;
	while (i>0){
		if(current_test > max_cur_test || current_test < min_cur_test) {
			//current_test = move_list(current_test, buts); //undefined reference... =(
		} else {
			y = (current_test - min_cur_test - 1) * 8;
			//current_test = change_pointer(y, current_test, buts); //undefined reference... =(
		}
		current_test = current_test_new;
		if( buts & BT_ENTER ){
			//__test_registry[current_test] -> run;
				TRACE ("SORRY\n");
		}
		i--;
	}
	return 0;
}
