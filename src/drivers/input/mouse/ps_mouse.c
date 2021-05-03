/**
 * @file
 *
 * @date May 1, 2021
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <drivers/input/input_dev.h>
#include <drivers/ps_mouse.h>

int ps_mouse_event_send(struct input_dev *indev, uint8_t data[3]) {
	struct input_event ev;

	ev.value = ((data[0] & MSTAT_XSIGN ? 0xff00 : 0) | data[1]) << 16;
	ev.value |= (data[0] & MSTAT_YSIGN ? 0xff00 : 0) | data[2];

	ev.type = 0;
	if (data[0] & MSTAT_BUTLEFT) {
		ev.type |= MOUSE_BUTTON_LEFT;
	}
	if (data[0] & MSTAT_BUTRIGHT) {
		ev.type  |= MOUSE_BUTTON_RIGHT;
	}
	if (data[0] & MSTAT_BUTMIDDLE) {
		ev.type  |= MOUSE_BUTTON_MIDDLE;
	}

	input_dev_report_event(indev, &ev);

	return 0;
}
