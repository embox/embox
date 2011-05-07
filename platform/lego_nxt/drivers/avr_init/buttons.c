/**
 * @file
 * @brief nxt buttons driver
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <drivers/nxt_avr.h>
#include <drivers/nxt_buttons.h>

static int buttons_state = 0;
static int buttons_accum_state = 0;

#define DEL 10
static int old_state = 0;
static int state_count = DEL;

/* Buttons pressed at now */
uint32_t nxt_buttons_are_pressed(void) {
	return buttons_state;
}

/* Buttons was pressed since last call */
uint32_t nxt_buttons_was_pressed(void) {
	uint32_t ret = buttons_accum_state;
	buttons_accum_state = 0;
	return ret;
}

static int translate_buttons(buttons_t buttons_val) {
	int ret = 0;
	if (buttons_val > 1500) {
		ret |= BT_ENTER;
		buttons_val -= 0x7ff;
	}

	if (buttons_val > 720) {
		ret |= BT_DOWN;
	} else if (buttons_val > 270) {
		ret |= BT_RIGHT;
	} else if (buttons_val > 60) {
		ret |= BT_LEFT;
	}

	return ret;
}

void buttons_updated(buttons_t buttons_val) {
	int new_state = translate_buttons(buttons_val);
	if (new_state == old_state) {
		if (!state_count) {
			/*button pressed*/
			buttons_state = new_state;
			buttons_accum_state |= new_state;
		} else {
			state_count--;
		}
	} else {
		if (!state_count) { /*button just released*/
		}
		old_state = new_state;
		state_count = DEL;
	}
}
