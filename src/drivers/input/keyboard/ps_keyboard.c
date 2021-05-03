/**
 * @file
 *
 * @date May 1, 2021
 * @author Anton Bondarev
 */

#include <drivers/input/input_dev.h>
#include <drivers/ps_keyboard.h>

static int kbd_state;

static void kbd_key_serv_press(int state, int flag) {
	if (state & KBD_KEY_PRESSED) {
		kbd_state |= flag;
	} else {
		kbd_state &= ~flag;
	}
}

int keyboard_scan_code_to_event(uint8_t scan_code, struct input_event *event) {
	int ret = 0, flag = 0;

	event->type = event->value = 0;

	if (scan_code & 0x80) {
		/* key unpressed */
		event->type &= ~KBD_KEY_PRESSED;
	} else {
		/* key pressed */
		event->type |= KBD_KEY_PRESSED;
	}
	scan_code &= 0x7F;

	switch(scan_code) {
	case KEYBOARD_SCAN_CODE_CTRL:
		flag = KBD_CTRL_PRESSED;
		break;
	case KEYBOARD_SCAN_CODE_ALT:
		flag = KBD_ALT_PRESSED;
		break;
	case KEYBOARD_SCAN_CODE_SHIFT:
		flag = KBD_SHIFT_PRESSED;
		break;
	case KEYBOARD_SCAN_CODE_CAPS:
		flag = KBD_CAPS_PRESSED;
		break;
	}
	kbd_key_serv_press(event->type, flag);

	event->value = kbd_state | scan_code;

	return ret;
}
