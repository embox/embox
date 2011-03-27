 /**
 * @file
 *
 * @date 10.12.2010
 * @author Darya Dzendzik
 */

#include <embox/test.h>
#include <drivers/lcd.h>
#include <types.h>

EMBOX_TEST(run_picture_embox_fill);


/*/extern int display_fill(uint8_t, uint8_t, uint8_t, uint8_t, int);*/

static int run_picture_embox_fill(void) {
	uint32_t b;
	display_clear_screen();
	b = nxt_buttons_was_pressed();

	if (b!=0) {
		return 0;
	}

	display_fill(6, 16, 12, 24, 1);
	display_fill(10, 20, 4, 8, 0);
	display_fill(10, 32, 8, 4, 0);
	display_fill(22, 16, 20, 24, 1);
	display_fill(26, 24, 4, 16, 0);
	display_fill(34, 24, 4, 16, 0);
	display_fill(26, 16, 16, 4, 0);

	display_fill(46, 8, 12, 32, 1);
	display_fill(50, 8, 8, 16, 0);
	display_fill(50, 28, 4, 8, 0);

	display_fill(62, 16, 12, 24, 1);
	display_fill(66, 20, 4, 16, 0);

	display_fill(78, 16, 16, 24, 1);
	display_fill(82, 16, 8, 8, 0);
	display_fill(82, 32, 8, 8, 0);
	display_fill(78, 24, 4, 8, 0);
	display_fill(90, 24, 4, 8, 0);


	for (int i = 0; i<5; i++) {
		b = nxt_buttons_was_pressed();
		if (b!=0) {
			return 0;
		}

		display_fill(6, 16, 12, 24, 0);
		display_fill(6, 8, 12, 24, 1);
		display_fill(10, 12, 4, 8, 0);
		display_fill(10, 24, 8, 4, 0);
		usleep(250);
		display_fill(6, 8, 12, 24, 0);
		display_fill(6, 16, 12, 24, 1);
		display_fill(10, 20, 4, 8, 0);
		display_fill(10, 32, 8, 4, 0);
		usleep(250);

		display_fill(22, 16, 20, 24, 0);
		display_fill(22, 8, 20, 24, 1);
		display_fill(26, 16, 4, 16, 0);
		display_fill(34, 16, 4, 16, 0);
		display_fill(26, 8, 16, 4, 0);
		usleep(250);
		display_fill(22, 8, 20, 24, 0);
		display_fill(22, 16, 20, 24, 1);
		display_fill(26, 24, 4, 16, 0);
		display_fill(34, 24, 4, 16, 0);
		display_fill(26, 16, 16, 4, 0);
		usleep(250);


		display_fill(46, 8, 12, 32, 0);
		display_fill(46, 0, 12, 32, 1);
		display_fill(50, 0, 8, 16, 0);
		display_fill(50, 20, 4, 8, 0);
		usleep(250);
		display_fill(46, 0, 12, 32, 0);
		display_fill(46, 8, 12, 32, 1);
		display_fill(50, 8, 8, 16, 0);
		display_fill(50, 28, 4, 8, 0);
		usleep(250);

		display_fill(62, 16, 12, 24, 0);
		display_fill(62, 8, 12, 24, 1);
		display_fill(66, 12, 4, 16, 0);
		usleep(250);
		display_fill(62, 8, 12, 24, 0);
		display_fill(62, 16, 12, 24, 1);
		display_fill(66, 20, 4, 16, 0);
		usleep(250);

		display_fill(78, 16, 16, 24, 0);
		display_fill(78, 8, 16, 24, 1);
		display_fill(82, 8, 8, 8, 0);
		display_fill(82, 26, 8, 14, 0);
		display_fill(78, 16, 4, 8, 0);
		display_fill(90, 16, 4, 8, 0);
		usleep(250);
		display_fill(78, 8, 16, 24, 0);
		display_fill(78, 16, 16, 24, 1);
		display_fill(82, 16, 8, 8, 0);
		display_fill(82, 32, 8, 8, 0);
		display_fill(78, 24, 4, 8, 0);
		display_fill(90, 24, 4, 8, 0);
		usleep(250);

	}
	usleep(100);
	return 0;
}
