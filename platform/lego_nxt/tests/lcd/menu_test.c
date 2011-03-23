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
extern void print_list_test(uint8_t cur_min_test);

static int run_menu_test(void){
	uint8_t y, current_test, max_cur_test, min_cur_test;
	int buts;
	int i = 1000000;
	current_test = 0;
	min_cur_test = 0;
	max_cur_test = 7;
	display_clear_screen();
	TRACE ("CCCccc EEEeee\n");
	usleep(200);
	display_clear_screen();
	usleep(200);

	print_list_test(0);
	usleep(2000);
	while (i>0){
		buts = nxt_buttons_was_pressed();
		if( (current_test > max_cur_test) || (current_test < min_cur_test) ) {
			current_test = move_list(current_test, buts);
			TRACE ("*************");
			usleep(200);
		} else {
			y = (current_test - min_cur_test) * 8;
			current_test = change_pointer(y, current_test, buts);
			TRACE ("!!!");
			TRACE ( current_test );
			usleep(200);
		}
		if( buts & BT_ENTER ){
			//__test_registry[current_test].run;
				TRACE ("SORRY\n");
				TRACE ("&&&&&&&&&&&&7");
				usleep(200);
		}
		i--;
		usleep(200);
	}
	return 0;
}
