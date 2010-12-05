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
	uint8_t buff[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	display_clear_screen();
	display_draw(70, 1, 1,	8, &buff[0]);
	display_draw(70, 3, 1,	8, &buff[0]);
	display_draw(70, 5, 1,	8, &buff[0]);
	display_draw(70, 7, 1,	8, &buff[0]);

	display_fill(50, 1, 8, 40, 1);
	display_fill(60, 3, 8, 40, 0);
	display_fill(80, 0, 8, 40, 1);
	display_fill(90, 9, 8, 40, 0);

	display_fill(0, 0, 1, 1, 1);
	display_fill(1, 1, 1, 1, 1);
	display_fill(2, 2, 1, 1, 1);
	display_fill(3, 3, 1, 1, 1);
	display_fill(4, 4, 1, 1, 1);
	display_fill(5, 5, 1, 1, 1);
	display_fill(6, 6, 1, 1, 1);
	display_fill(7, 7, 1, 1, 1);



	display_draw(10, 1, 1,	8, &buff[0]);
	display_draw(10, 3, 1,	8, &buff[0]);
	display_draw(10, 5, 1,	8, &buff[0]);
	display_draw(10, 7, 1,	8, &buff[0]);

	//usleep(800);

	display_fill(20, 0, 1, 1, 0);
	display_fill(21, 1, 1, 1, 0);
	display_fill(22, 2, 1, 1, 0);
	display_fill(23, 3, 1, 1, 0);
	display_fill(24, 4, 1, 1, 0);
	display_fill(25, 5, 1, 1, 0);
	display_fill(26, 6, 1, 1, 0);
	display_fill(27, 7, 1, 1, 0);
	usleep(800000);


	/*for (i=0; i<10 ;i++) {
		j=i%2;
		display_fill(5*i, 5*i, 5*i, 5*i, j);
		usleep(1000);
	}*/

	return 0;
}

