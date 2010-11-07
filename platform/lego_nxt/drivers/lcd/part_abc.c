/**
 * @file
 *
 * @date 6.11.10
 * @author Darya Dzendzik
 */

#include <hal/reg.h>
#include <util/math.h>
#include <drivers/at91sam7s256.h>
#include <drivers/lcd.h>
#include <string.h>


extern int display_draw(uint8_t, uint8_t, uint8_t, uint8_t, char *);

static int part_abc_init(void)
{
/*clean letter*/
	char pict_buff0[0] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

/*Part of "E" "M" "B" "O" */
	char pict_buff1[8] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
	char pict_buff2[8] = {0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0};
	char pict_buff4[8] = {0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF};
	char pict_buff3[8] = {0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF};

/*part of "X"*/
	char pict_buff5[8] = {0xC3, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3};
	char pict_buff8[8] = {0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xFC, 0x7F};
	char pict_buff9[8] = {0xFF, 0x3F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00};
	char pict_buff7[8] = {0xFF, 0xFC, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00};
	char pict_buff6[8] = {0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x3F, 0xFF};
	char pict_buff10[8] = {0xFF, 0x3F, 0x0F, 0x03, 0x03, 0x0F, 0x3F, 0xFF};
	char pict_buff11[8] = {0xFF, 0xFC, 0xF0, 0xC0, 0xC0, 0xF0, 0xFC, 0xFF};

	return 0;
}

void display_b(uint8_t x, uint8_t y){
	display_draw(x, y-1, 1, 8, &pict_buff1[0]);
	display_draw(x, y, 1, 8, &pict_buff1[0]);
	display_draw(x, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+1, 1, 8, &pict_buff3[0]);
	display_draw(x, y+2, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+2, 1, 8, &pict_buff4[0]);
}

void display_e(uint8_t x, uint8_t y){
	display_draw(x, y, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y, 1, 8, &pict_buff3[0]);
	display_draw(x, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+1, 1, 8, &pict_buff4[0]);
	display_draw(x, y+2, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+2, 1, 8, &pict_buff2[0]);
}

void display_m(uint8_t x, uint8_t y){
	display_draw(x, y, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y, 1, 8, &pict_buff2[0]);
	display_draw(x+16, y, 1, 8, &pict_buff2[0]);
	display_draw(x, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x+16, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x, y+2, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+2, 1, 8, &pict_buff1[0]);
	display_draw(x+16, y+2, 1, 8, &pict_buff1[0]);
}

void display_o(uint8_t x, uint8_t y){
	display_draw(x, y, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y, 1, 8, &pict_buff3[0]);
	display_draw(x, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+1, 1, 8, &pict_buff1[0]);
	display_draw(x, y+2, 1, 8, &pict_buff1[0]);
	display_draw(x+8, y+2, 1, 8, &pict_buff4[0]);
}

void display_x(uint8_t x, uint8_t y){
	display_draw(x, y, 1, 8, &pict_buff6[0]);
	display_draw(x+1, y, 1, 8, &pict_buff11[0]);
	display_draw(x+2, y, 1, 8, &pict_buff9[0]);
	display_draw(x+1, y+1, 1, 8, &pict_buff5[0]);
	display_draw(x, y+2, 1, 8, &pict_buff8[0]);
	display_draw(x+1, y+2, 1, 8, &pict_buff10[0]);
	display_draw(x+2, y+2, 1, 8, &pict_buff7[0]);
}

void display_clean_sp(uint8_t x, uint8_t y, int i, int j){
	for(int t=0; t<i; t++){
		for (int k=0; k<j; k++)
		{
			display_draw(x+t, y+k, 1, 8, &pict_buff0[0]);
		}
	}
}
