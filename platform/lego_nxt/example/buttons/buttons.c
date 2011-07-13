/**
 * @file
 * @brief nxt buttons test
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/example.h>
#include <unistd.h>
#include <drivers/nxt/buttons.h>

EMBOX_EXAMPLE(nxt_buttons_example);

static int nxt_buttons_example(void) {
	while (true) {
		if (nxt_buttons_pressed()) {
			break;
		}
		usleep(200);
	}
	return 0;
}
