/**
 * @file
 * @brief nxt buttons test
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <unistd.h>

#include <drivers/nxt_buttons.h>

EMBOX_TEST(nxt_buttons_test);

static int nxt_buttons_test(void) {
	while (true) {
		if (nxt_buttons_was_pressed()) {
			break;
		}
		usleep(200);
	}
	return 0;
}

