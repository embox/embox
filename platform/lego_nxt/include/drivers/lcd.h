/**
 * @file
 *
 * @date 11.10.2010
 * @author Nikolay Korotky
 */

#include <types.h>

#ifndef LCD_H_
#define LCD_H_

#define SPI_BITRATE     2000000
#define CLOCK_FREQUENCY 48054850

#define NXT_LCD_DEPTH 8
#define NXT_LCD_WIDTH 100

#define N_CHARS            128
#define FONT_WIDTH         5
#define CELL_WIDTH         (FONT_WIDTH + 1)
#define DISPLAY_CHAR_WIDTH (NXT_LCD_WIDTH/(CELL_WIDTH))
#define DISPLAY_CHAR_DEPTH (NXT_LCD_DEPTH)

extern void display_char(int c);
extern void display_string(const char *str);

extern void nxt_lcd_force_update(void);

extern int display_draw(uint8_t x, uint8_t y,
	   	uint8_t width, uint8_t height, uint8_t *buff);

#endif /* LCD_H_ */
