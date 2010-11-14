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

	char pict_buff1[8] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};

	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	usleep(500);
	display_fill(10, 10, 80, 50, 1);
	usleep(1000);
	display_fill(15, 15, 10, 1, 0);
	usleep(500);


	/*for(i=0; i<10 ;i++) {
		j=i%2;
		display_fill(5*i, 5*i, 5*i, 5*i, j);
		usleep(1000);
	}*/

	return 0;
}

