/**
 * @file
 *
 * @date Apr 28, 2021
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>

#include <kernel/irq.h>

#include <drivers/input/input_dev.h>
#include <drivers/common/memory.h>

#include <drivers/pl050.h>

#include <embox/unit.h>

#define BASE_ADDR OPTION_GET(NUMBER,base_addr)
#define IRQ_NUM   OPTION_GET(NUMBER,irq_num)

EMBOX_UNIT_INIT(pl050_keyboard_init);


struct pl050_keyboard_indev {
	struct input_dev input_dev;
	struct pl050 *pl050_dev;
};

static int pl050_keyboard_stop(struct input_dev *dev) {

	/* TODO */
	return 0;
}

static const struct input_dev_ops kbd_input_ops = {
		/*.start = keyboard_start,*/
		.stop = pl050_keyboard_stop,
};

static struct pl050_keyboard_indev keyboard_dev = {
	.input_dev = {
		.ops = &kbd_input_ops,
		.name = "ps-keyboard",
		.type = INPUT_DEV_KBD,
	},
};

static int kbd_state;

static void kbd_key_serv_press(int state, int flag) {
	if (state & KBD_KEY_PRESSED) {
		kbd_state |= flag;
	} else {
		kbd_state &= ~flag;
	}
}

static int keyboard_scan_code_to_event(uint8_t scan_code, struct input_event *event) {
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

static irq_return_t pl050_keyboard_irq_hnd(unsigned int irq_nr, void *data) {
	struct pl050_keyboard_indev *pl050_keyboard_indev;
	struct pl050 *pl050;
	uint32_t rx_data;
	struct input_event event;
	struct input_dev *indev;

	pl050_keyboard_indev = (struct pl050_keyboard_indev *)data;
	pl050 = pl050_keyboard_indev->pl050_dev;
	indev = &pl050_keyboard_indev->input_dev;

	rx_data = pl050->data;

	if (rx_data == KEYBOARD_SCAN_CODE_EXT) {
		goto out;
	}

	keyboard_scan_code_to_event(rx_data, &event);
	input_dev_report_event(indev, &event);

out:
	return IRQ_HANDLED;
}

static int pl050_keyboard_init(void) {
	int res;

	res = irq_attach(IRQ_NUM, pl050_keyboard_irq_hnd, 0,
					 &keyboard_dev, "ps keyboard");
	if (res < 0) {
		return res;
	}
	keyboard_dev.pl050_dev = (void*)(uintptr_t)BASE_ADDR;

	pl050_init(keyboard_dev.pl050_dev);

	return input_dev_register(&keyboard_dev.input_dev);
}

PERIPH_MEMORY_DEFINE(pl050_keyboard, BASE_ADDR, 0x1000);
