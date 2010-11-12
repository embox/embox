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

/*Part of "E" "M" "B" "O" */
static uint8_t pict_buff1[8] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t pict_buff2[8] = {0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0};
static uint8_t pict_buff4[8] = {0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t pict_buff3[8] = {0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF};
 /*part of "X"*/
static uint8_t pict_buff5[8] = {0xC3, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3};
static uint8_t pict_buff8[8] = {0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xFC, 0x7F};
static uint8_t pict_buff9[8] = {0xFF, 0x3F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00};
static uint8_t pict_buff7[8] = {0xFF, 0xFC, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00};
static uint8_t pict_buff6[8] = {0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x3F, 0xFF};
static uint8_t pict_buff10[8] = {0xFF, 0x3F, 0x0F, 0x03, 0x03, 0x0F, 0x3F, 0xFF};
static uint8_t pict_buff11[8] = {0xFF, 0xFC, 0xF0, 0xC0, 0xC0, 0xF0, 0xFC, 0xFF};

static void display_b(uint8_t x, uint8_t y){
	display_draw(x, y-1, 1, 8, &pict_buff1[0]);
	display_draw(x, y, 1, 8, &pict_buff1[0]);
	display_draw(x, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+1, 1, 8, &pict_buff3[0]);
	display_draw(x, y+2, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+2, 1, 8, &pict_buff4[0]);
	nxt_lcd_force_update();
}

static void display_e(uint8_t x, uint8_t y){
	display_draw(x, y, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y, 1, 8, &pict_buff3[0]);
	display_draw(x, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+1, 1, 8, &pict_buff4[0]);
	display_draw(x, y+2, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+2, 1, 8, &pict_buff2[0]);
	nxt_lcd_force_update();
}

static void display_m(uint8_t x, uint8_t y){
	display_draw(x, y, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y, 1, 8, &pict_buff2[0]);
	display_draw(x+16, y, 1, 8, &pict_buff2[0]);
	display_draw(x, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x+16, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x, y+2, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+2, 1, 8, &pict_buff1[0]);
	display_draw(x+16, y+2, 1, 8, &pict_buff1[0]);
	nxt_lcd_force_update();
}

static void display_o(uint8_t x, uint8_t y){
	display_draw(x, y, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y, 1, 8, &pict_buff3[0]);
	display_draw(x, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x, y+2, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+2, 1, 8, &pict_buff4[0]);
	nxt_lcd_force_update();
}

static void display_x(uint8_t x, uint8_t y){
	display_draw(x, y, 1, 8, &pict_buff6[0]);
	display_draw(x+8, y, 1, 8, &pict_buff11[0]);
	display_draw(x+16, y, 1, 8, &pict_buff9[0]);
	display_draw(x+8, y+1, 1, 8, &pict_buff5[0]);
	display_draw(x, y+2, 1, 8, &pict_buff8[0]);
	display_draw(x+8, y+2, 1, 8, &pict_buff10[0]);
	display_draw(x+16, y+2, 1, 8, &pict_buff7[0]);
	nxt_lcd_force_update();
}

static void display_clean_sp(uint8_t x, uint8_t y, int i, int j){
	for(int t=0; t<i; t++){
		for (int k=0; k<j; k++)
		{
			display_draw(x+t, y+k, 1, 8, &pict_buff0[0]);
		}
	}
	nxt_lcd_force_update();
}
static int run_abc_embox(void) {
	display_clear_screen();
	display_e(2, 3);
	display_m(18, 3);
	display_b(42, 3);
	display_o(58, 3);
	display_x(74, 3);
	usleep(500);
	/*jamp*/
	display_clean_sp(2, 3, 13, 3);
	display_e(2, 2);
	usleep(500);
	display_clean_sp(2, 2, 13, 3);
	display_e(2, 3);
	usleep(500);

	display_clean_sp(18, 3, 13, 3);
	display_clean_sp(22, 3, 13, 3);
	display_m(18, 2);
	usleep(500);
	display_clean_sp(18, 2, 13, 3);
	display_clean_sp(22, 2, 13, 3);
	display_m(18, 3);
	usleep(500);

	display_clean_sp(42, 3, 13, 3);
	display_b(42, 2);
	usleep(500);
	display_clean_sp(42, 1, 13, 3);
	display_clean_sp(42, 2, 13, 3);
	display_b(42, 3);
	usleep(500);

	display_clean_sp(58, 3, 13, 3);
	display_o(58, 2);
	usleep(500);
	display_clean_sp(58, 2, 13, 3);
	display_o(58, 3);
	usleep(500);

	display_clean_sp(74, 3, 13, 3);
	display_x(74, 2);
	usleep(500);
	display_clean_sp(74, 2, 13, 3);
	display_x(74, 3);

	return 0;
}

