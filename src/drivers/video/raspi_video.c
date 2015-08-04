/**
 * @file
 * @brief VideoCore Device Driver for Raspberry Pi
 *
 * @date 16.07.15
 * @author Michele Di Giorgio
 */

#include <stdint.h>
#include <defines/null.h>
#include <drivers/video/raspi_fb.h>

#include <embox/unit.h>

int rows, cols;
int cursor_row;
int cursor_col;

static int this_init(void) {

	init_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT, BIT_DEPTH);

	// Initialize screen global variables
	rows = SCREEN_HEIGHT/CHAR_HEIGHT;
	cols = SCREEN_WIDTH/CHAR_WIDTH;
	cursor_row = cursor_col = 0;
	draw_string("Hello Embox!\n", 0x52345678);

	return 0;
}

EMBOX_UNIT_INIT(this_init);
