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
static uint8_t space_buff[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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
void print_list_test(int first){
	for (int i = 0; i<8; i++){
		tab_displey( __test_registry[first + i].mod->name );
	}
}

/*This function move list of test */
uint8_t move_list_up(int current_test){
	if (current_test >= 0) {
		print_list_test(current_test);
	} else {
		current_test = 0;
	}
	return current_test;
}

int move_list_down(int current_test, int number ){
		if(current_test <= number){
			print_list_test(current_test - 8);
		} else {
			current_test = number;
		}
	return current_test;
}
/*void execution(int current_test) {
	__test_registry[current_test].test_cases[current_test]->run;
	//__test_registry[current_test].test_cases[0]->run;
}*/

int menu_start(void){
	int current_test, max_cur_test, min_cur_test;
	int j, i, buts, number;
	number = n_of_t();
	current_test = 0;
	min_cur_test = 0;
	max_cur_test = 7;
	i = 1;
	print_list_test(current_test);
	display_draw(0, 0, 1, 1, &pointer_buff[0]);

	while (i>0){
		buts = nxt_buttons_was_pressed();
		usleep(400);
		if (buts & BT_RIGHT){
			current_test++;
			if (current_test > max_cur_test) {
				current_test = move_list_down( current_test, number );
				display_draw(0, 7, 1, 8, &pointer_buff[0]);
				if ( max_cur_test < number ){
					min_cur_test++;
					max_cur_test++;
				}
			} else {
				display_draw(0, current_test, 1, 8, &pointer_buff[0]);
				display_draw(0, current_test-1, 1, 8, &space_buff[0]);
			}
		}
		if (buts & BT_LEFT){
			current_test--;
			if( current_test < min_cur_test ) {
				current_test = move_list_up( current_test );
				display_draw(0, 0, 1, 8, &pointer_buff[0]);
				if ( min_cur_test > 0 ){
					min_cur_test--;
					max_cur_test--;
				}
			} else {
				display_draw(0, current_test, 1, 8, &pointer_buff[0]);
				display_draw(0, current_test+1, 1, 8, &space_buff[0]);
			}
		}
		if( buts & BT_ENTER ){
			j = test_suite_run( &__test_registry[current_test]);
		}
		if (buts & BT_DOWN ){
			i = 0;
		}
		buts = 0;
	}
	return 0;
}
