/**
 * @file
 *
 * @date 06.12.10
 * @author Darya Dzendzik
 */

#include <types.h>
#include <unistd.h>

#include <embox/example.h>
#include <drivers/nxt/lcd.h>

EMBOX_EXAMPLE(run_new_fill);

/*extern int display_fill(uint8_t, uint8_t, uint8_t, uint8_t, int);*/

static int run_new_fill(void) {
	int i, j, t;

	for (i = 0; i < 13; i++) {
		t = i % 2;
		for (j = 0; j < 16; j++) {
			display_fill(i * 4, j * 4, 100 - i * 8, 32 - j * 8, t);
		}

	}
	usleep(2000);

	display_fill(0, 0, 100, 64, 1);
	usleep(200);
	display_fill(4, 4, 92, 56, 0);
	usleep(200);
	display_fill(8, 8, 84, 48, 1);
	usleep(200);
	display_fill(12, 12, 76, 40, 0);
	usleep(200);
	display_fill(16, 16, 68, 32, 1);
	usleep(200);
	display_fill(20, 20, 60, 24, 0);
	usleep(200);
	display_fill(24, 24, 52, 16, 1);
	usleep(200);
	display_fill(28, 28, 44, 8, 0);
	usleep(200);
	display_fill(32, 31, 36, 2, 1);
	usleep(2000);

	for (i = 0; i < 8; i++) {
		t = (i % 2)+1;
		display_fill( i*4, i*4, (100 - i*8), (64 - i*8), t);
		usleep(1000);
	}
	//display_fill(32, 34, 36, 2, 1);

	usleep(2000);

	/*for (i = 0; i < 1; i++) {
		t = (i % 2);
		display_fill( i * 4, i * 4, 100 - i*8, 64 - i*8, t);
		usleep(500);
	}
	display_fill(32, 36, 36, 2, 0);
	usleep(2000000);*/

	return 0;
}
