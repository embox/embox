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
static uint8_t pointer_buff[8] = {0x00, 0x18, 0x3C, 0x7E, 0x7E, 0x3C, 0x18, 0x00};
//__u8 display_buffer[NXT_LCD_DEPTH + 1][NXT_LCD_WIDTH];

static int run_menu_test(void){
	uint8_t y, current_test, max_cur_test, min_cur_test;
	int buts;
	int i = 1000000;
	current_test = 0;
	min_cur_test = 0;
	max_cur_test = 7;
	display_clear_screen();
	print_list_test(current_test);
	display_draw(0, 0, 1, 1, &pointer_buff[0]);
	while (1>0){
		buts = nxt_buttons_was_pressed();
		if( (current_test > max_cur_test) | (current_test < min_cur_test) ) {
			current_test = move_list(current_test, buts);
		} else {
			y = (current_test - min_cur_test) * 8;
			current_test = change_pointer(y, current_test, buts);
		}
		if( buts & BT_ENTER ){
			//__test_registry[current_test].run;
			//__test_registry[current_test].
			TRACE ("SORRY\n");
		}
		i--;
		buts = 0;
	}
	return 0;
}
