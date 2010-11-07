/**
 * @file
 *
 * @date 17.10.2010
 * @author Darya Dzendzik
 */

#include <embox/test.h>
#include <drivers/lcd.h>
#include <types.h>

EMBOX_TEST(run_abc_embox);

extern void display_e(uint8_t x, uint8_t y);
extern void display_m(uint8_t x, uint8_t y);
extern void display_b(uint8_t x, uint8_t y);
extern void display_o(uint8_t x, uint8_t y);
extern void display_x(uint8_t x, uint8_t y);
extern void display_clean_sp(uint8_t x, uint8_t y, int i, int j);

static int run_abc_embox(void) {
	part_abc_init();
	display_e(2, 2);
	display_m(18, 2);
	display_b(42, 2);
	display_o(58, 2);
	display_x(74, 2);
	usleep(500);
	/*jamp*/
	display_clean_sp(2, 2, 16, 3);
	display_e(2, 1);
	usleep(550);
	display_clean_sp(2, 1, 16, 3);
	display_e(2, 2);
	usleep(550);
	return 0;
}

