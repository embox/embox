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
#include <drivers/ps_keyboard.h>
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
