/**
 * @file
 *
 * @date 14.10.10
 * @author Darya Dzendzik
 */

#include <embox/example.h>
#include <drivers/nxt/lcd.h>

/* wont to see some picture, for example black sq*/

EMBOX_EXAMPLE(run_picture);

static int run_picture(void) {
	char pict_buff[8] = {0x00, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x00};
	display_draw(0, 0, 1, 8, &pict_buff[0]);
	return 0;
}
