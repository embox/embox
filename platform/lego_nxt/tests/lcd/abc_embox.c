/**
 * @file
 *
 * @date 05.11.2010
 * @author Darya Dzendzik
 */

#include <embox/test.h>
#include <drivers/lcd.h>
#include <unistd.h>
#include <types.h>

EMBOX_TEST(run_abc_embox);

static uint8_t pict_buff0[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*Part of "E" "M" "B" "O" "X"*/
static uint8_t pict_buff1[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0x0F, 0x0F};
static uint8_t pict_buff2[8] = {0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t pict_buff3[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xF0, 0xF0};
static uint8_t pict_buff4[8] = {0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t pict_buff5[8] = {0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0};
static uint8_t pict_buff6[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF};
static uint8_t pict_buff7[8] = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF};
static uint8_t pict_buff8[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
static uint8_t pict_buff9[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t pict_buff10[8] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t pict_buff11[8] = {0x03, 0x0F, 0x3F, 0xFF, 0xFF, 0xFC, 0xF0, 0xC0};
static uint8_t pict_buff12[8] = {0xC0, 0xF0, 0xFC, 0xFF, 0xFF, 0x3F, 0x0F, 0x03};
static uint8_t pict_buff13[8] = {0xC3, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3};

static void display_e(uint8_t x, uint8_t y) {
	display_draw(x, y, 1, 8, &pict_buff1[0]);
	display_draw(x + 8, y, 1, 8, &pict_buff2[0]);
	display_draw(x, y + 1, 1, 8, &pict_buff3[0]);
	display_draw(x + 8, y + 1, 1, 8, &pict_buff4[0]);
	display_draw(x, y + 2, 1, 8, &pict_buff3[0]);
	display_draw(x + 8, y + 2, 1, 8, &pict_buff5[0]);
	nxt_lcd_force_update();
}

static void display_b(uint8_t x, uint8_t y) {
	display_draw(x, y, 1, 8, &pict_buff1[0]);
	display_draw(x + 8, y, 1, 8, &pict_buff8[0]);
	display_draw(x, y + 1, 1, 8, &pict_buff9[0]);
	display_draw(x + 8, y + 1, 1, 8, &pict_buff3[0]);
	display_draw(x, y + 2, 1, 8, &pict_buff3[0]);
	display_draw(x + 8, y + 2, 1, 8, &pict_buff4[0]);
	nxt_lcd_force_update();
}

static void display_m(uint8_t x, uint8_t y) {
	display_draw(x, y, 1, 8, &pict_buff3[0]);
	display_draw(x + 8, y, 1, 8, &pict_buff5[0]);
	display_draw(x, y + 1, 1, 8, &pict_buff6[0]);
	display_draw(x + 8, y + 1, 1, 8, &pict_buff7[0]);
	display_draw(x, y + 2, 1, 8, &pict_buff6[0]);
	display_draw(x + 8, y + 2, 1, 8, &pict_buff7[0]);
	nxt_lcd_force_update();
}

static void display_o(uint8_t x, uint8_t y) {
	display_draw(x, y, 1, 8, &pict_buff1[0]);
	display_draw(x + 8, y, 1, 8, &pict_buff2[0]);
	display_draw(x, y + 1, 1, 8, &pict_buff8[0]);
	display_draw(x + 8, y + 1, 1, 8, &pict_buff10[0]);
	display_draw(x, y + 2, 1, 8, &pict_buff3[0]);
	display_draw(x + 8, y + 2, 1, 8, &pict_buff4[0]);
	nxt_lcd_force_update();
}

static void display_x(uint8_t x, uint8_t y) {
	display_draw(x, y, 1, 8, &pict_buff11[0]);
	display_draw(x + 8, y, 1, 8, &pict_buff12[0]);
	display_draw(x + 4, y + 1, 1, 8, &pict_buff13[0]);
	display_draw(x, y + 2, 1, 8, &pict_buff12[0]);
	display_draw(x + 8, y + 2, 1, 8, &pict_buff11[0]);
	nxt_lcd_force_update();
}

static void display_clean_sp(uint8_t x, uint8_t y) {
	size_t t, k;
	for (t = 0; t < 10; t++) {
		for (k = 0; k < 3; k++) {
			display_draw(x + t, y + k, 1, 8, &pict_buff0[0]);
		}
	}
	nxt_lcd_force_update();
}

static int run_abc_embox(void) {
	display_clear_screen();
	display_e(2, 3);
	display_m(22, 3);
	display_b(42, 3);
	display_o(60, 3);
	display_x(80, 3);
	usleep(250);
	/*jamp*/
	display_clean_sp(2, 3);
	display_e(2, 2);
	usleep(250);
	display_clean_sp(2, 2);
	display_e(2, 3);
	usleep(250);

	display_clean_sp(22, 3);
	display_m(22, 2);
	usleep(250);
	display_clean_sp(22, 2);
	display_m(22, 3);
	usleep(250);

	display_clean_sp(42, 3);
	display_b(42, 2);
	usleep(250);
	display_clean_sp(42, 2);
	display_b(42, 3);
	usleep(250);

	display_clean_sp(60, 3);
	display_o(60, 2);
	usleep(250);
	display_clean_sp(60, 2);
	display_o(60, 3);
	usleep(250);

	display_clean_sp(80, 3);
	display_x(80, 2);
	usleep(250);
	display_clean_sp(80, 2);
	display_x(80, 3);

	return 0;
}

