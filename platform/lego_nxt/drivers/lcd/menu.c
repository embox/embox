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
/*extern int display_fill(uint8_t, uint8_t, uint8_t, uint8_t, int);*/

/*pointer*/
static uint8_t pointer_buff[8] = {0x00, 0x18, 0x3C, 0x7E, 0x7E, 0x3C, 0x18, 0x00};

/* This function change pointer place
 * then list of test don't move
 */
uint8_t change_pointer(uint8_t y, uint8_t current_test, int buts){
	if (buts & BT_LEFT) {
		display_buffer[0][y+8] = pointer_buff[0];
		current_test++;
	}
	if (buts & BT_RIGHT) {
		display_buffer[0][y-8] = pointer_buff[0];
		current_test--;
	}
	display_fill(0, y, 8, 8, 0);
	nxt_lcd_force_update();
	return current_test;
}

/*number of test*/
int n_of_t(void){
	const struct test_suite *test;
	int i = 0;
	test_foreach(test) {
		++i;
	}
	printf("\nTotal tests: %d\n", i);

	/*const struct test *test;
	uint8_t n;
	n = 0;
	test_foreach(test){
		n = n + 1;
	}*/
	return i;
}

/*This function move list of test */
uint8_t move_list(uint8_t current_test, int buts){
	int number, i;

	/*for (i = 0; i<8; i++){
		TRACE("  /n");
		TRACE( __test_registry[i].mod->name ); //hm... maybe something else
	}*/

	if (buts & BT_LEFT) {
		number = n_of_t() + 1;
		for (i = 0; i<8; i++){
			while(current_test < number){
				TRACE("  /n");
				TRACE( __test_registry[i].mod->name);//->mod.name); //hm... maybe something else
			}
		}
		current_test++;
	}
	if (buts & BT_RIGHT) {
		for (int i = 0; i<8; i++){
			while(current_test > 1){
				TRACE("  /n");
				TRACE(__test_registry[current_test-1+i].mod->name); //hm... maybe something else
			}
		}
		current_test--;
	}
	nxt_lcd_force_update();
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
