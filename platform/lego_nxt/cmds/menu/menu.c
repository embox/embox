/**
 * @file
 * @brief primitive graphic menu for LEGO
 *
 * @date 11.03.11
 * @author Darya Dzendzik
 */

#include <unistd.h>
#include <drivers/menu.h>
#include <drivers/lcd.h>
#include <drivers/nxt_buttons.h>
#include <framework/test/api.h>

/*pointer*/
static uint8_t pointer_buff[8] = {0x00, 0x18, 0x3C, 0x7E, 0x7E, 0x3C, 0x18, 0x00};
static uint8_t space_buff[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Get number of test*/
static int n_of_t(void) {
	const struct test_suite *test;
	int i = 0;
	test_foreach(test) {
		++i;
	}
	return i;
}

/*This function print list of test on lcd  */
static void print_list_test(int first) {
	size_t i;
	for (i = 0; i<8; i++) {
		tab_display( __test_registry[first + i].mod->name );
	}
}

int menu_start(void) {
	int current_test, max_cur_test, min_cur_test;
	int j, i, buts, number;
	number = n_of_t();
	current_test = 0;
	min_cur_test = 0;
	max_cur_test = 7;
	i = 1;
	print_list_test(current_test);
	display_draw(0, 0, 1, 1, &pointer_buff[0]);

	while (i > 0) {
		//buts = nxt_buttons_was_pressed();
		usleep(600);
		if (buts & BT_RIGHT) {
			if (current_test == max_cur_test) {
				if (max_cur_test < (number-1)) {
					min_cur_test++;
					max_cur_test++;
					current_test++;
					display_clear_screen();
					print_list_test(current_test - 7);
					display_draw(0, 7, 1, 8, &pointer_buff[0]);
				}
			} else {
				current_test++;
				display_draw(0, current_test - min_cur_test, 1, 8, &pointer_buff[0]);
				display_draw(0, current_test - 1 - min_cur_test, 1, 8, &space_buff[0]);
			}
		}
		if (buts & BT_LEFT) {
			if (current_test == min_cur_test) {
				if ( min_cur_test > 0 ) {
					min_cur_test--;
					max_cur_test--;
					current_test--;
					display_clear_screen();
					print_list_test(current_test);
					display_draw(0, 0, 1, 8, &pointer_buff[0]);
				}
			} else {
				current_test--;
				display_draw(0, current_test - min_cur_test, 1, 8, &pointer_buff[0]);
				display_draw(0, current_test + 1 - min_cur_test, 1, 8, &space_buff[0]);
			}
		}
		if ( buts & BT_ENTER) {
			j = test_suite_run(&__test_registry[current_test]);
		}
		if (buts & BT_DOWN) {
			i = 0;
		}
		buts = 0;
	}
	return 0;
}
