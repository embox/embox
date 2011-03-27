/**
 * @file
 * @brief primitive graphic menu for LEGO
 *
 * @date 11.03.11
 * @author Darya Dzendzik
 */

#include <hal/reg.h>
#include <util/math.h>
#include <drivers/at91sam7s256.h>
#include <drivers/lcd.h>
#include <string.h>
#include <drivers/menu.h>
#include <drivers/nxt_buttons.h>

#include <framework/test/api.h>
#include <embox/cmd.h>
//#include <src/include/getopt.h>
//#include <src/include/stdio.h>
/*#include <src/include/test/framework.h>*/

extern __u8 display_buffer[NXT_LCD_DEPTH+1][NXT_LCD_WIDTH];

/*pointer*/
static uint8_t pointer_buff[8] = {0x00, 0x18, 0x3C, 0x7E, 0x7E, 0x3C, 0x18, 0x00};

/* This function change pointer place
 * then list of test don't move
 */
uint8_t change_pointer(uint8_t y, uint8_t current_test, int buts){
	if (buts & BT_LEFT) {
		display_draw(0, y-1, 1, 1, &pointer_buff[0]);
		display_fill(0, y, 8, 8, 0);
		current_test--;
	}
	if (buts & BT_RIGHT) {
		display_draw(0, y+1, 1, 1, &pointer_buff[0]);
		display_fill(0, y, 8, 8, 0);
		current_test++;
	}
	return current_test;
}

/* Get number of test*/
int n_of_t(void){
	const struct test_suite *test;
	int i = 0;
	test_foreach(test) {
		++i;
	}
	return i;
}

/*This function print list of test on lcd  */
void print_list_test(uint8_t first){
	for (int i = 0; i<8; i++){
		tab_displey( __test_registry[first + i].mod->name); //hm... maybe something else
	}
}

/*This function move list of test */
uint8_t move_list(uint8_t current_test, int buts){
	int number, i;

	if (buts & BT_LEFT) {
		number = n_of_t();
		for (i = 0; i<8; i++){
			if(current_test < number){
				tab_displey( __test_registry[current_test+8-i].mod->name);
			}
		}
		current_test++;
	}
	if (buts & BT_RIGHT) {
		current_test--;
		for (int i = 0; i<8; i++){
			if(current_test > 0){
				tab_displey(__test_registry[current_test+i].mod->name);
			}
		}
	}
	return current_test;
}

/*void execution(current_test){
//	int buts = nxt_buttons_was_pressed();
	const struct test *test;
	if (buts & BT_ENTER){
		 __test_registry[current_test] -> run;
	}
	nxt_lcd_force_update();
	return 0;
}*/
