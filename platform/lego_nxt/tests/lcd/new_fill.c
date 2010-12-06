
/**
 * @file
 *
 * @date 06.12.2010
 * @author Darya Dzendzik
 */

#include <embox/test.h>
#include <drivers/lcd.h>
#include <types.h>

EMBOX_TEST(run_new_fill);

extern int display_fill(uint8_t, uint8_t, uint8_t, uint8_t, int);

static int run_new_fill(void) {
	int i, j, t;
	for (i = 0; i < 25; i++) {
		t = i % 2;
		for( j = 0; j<16; j++){
			display_fill( i * 4, j * 4, 100 - i*8, 64 - j*8, t);
			usleep(200);
		}

	}
	display_fill(32, 34, 36, 2, 0);
	usleep(2000);

	display_fill(0, 0, 100, 64, 0);

	usleep(2000);

	for (i = 0; i < 8; i++) {
		t = (i % 2)+1;
		display_fill( i * 4, i * 4, 100 - i*8, 64 - i*8, t);
		usleep(500);
	}
	display_fill(32, 34, 36, 2, 1);

	usleep(2000);

	for (i = 0; i < 8; i++) {
		t = (i % 2);
		display_fill( i * 4, i * 4, 100 - i*8, 64 - i*8, t);
		usleep(500);
	}
	display_fill(32, 36, 36, 2, 0);
	usleep(2000000);

	return 0;
}
