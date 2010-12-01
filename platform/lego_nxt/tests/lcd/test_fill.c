/**
 * @file
 *
 * @date 21.10.2010
 * @author Darya Dzendzik
 */

#include <embox/test.h>
#include <drivers/lcd.h>
#include <types.h>

EMBOX_TEST(run_fill);

extern int display_fill(uint8_t, uint8_t, uint8_t, uint8_t, int);

static int run_fill(void) {
	//int i, j;
	display_clear_screen();
	char pict_buff1[8] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
	display_fill(10, 10, 40, 10, 1);
	usleep(1000);
	display_fill(15, 16, 10, 1, 0);
	usleep(1000);
	display_fill(1, 2, 1, 1, 1);
	display_fill(2, 3, 1, 1, 1);
	display_fill(3, 4, 1, 1, 1);
	display_fill(4, 5, 1, 1, 1);
	display_fill(5, 6, 1, 1, 1);
	display_fill(6, 7, 1, 1, 1);
	usleep(2000000);


	/*for (i=0; i<10 ;i++) {
		j=i%2;
		display_fill(5*i, 5*i, 5*i, 5*i, j);
		usleep(1000);
	}*/

	return 0;
}

