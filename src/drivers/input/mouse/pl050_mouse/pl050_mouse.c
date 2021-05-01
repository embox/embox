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

#define MSTAT_BUTMASK   0x07
#define MSTAT_BUTLEFT   0x01
#define MSTAT_BUTRIGHT  0x02
#define MSTAT_BUTMIDDLE 0x04
#define MSTAT_ALONE     0x08
#define MSTAT_XSIGN     0x10
#define MSTAT_YSIGN     0x20
#define MSTAT_XOVER     0x40
#define MSTAT_YOVER     0x80

#define MOUSE_ACK       0xfa

#define BASE_ADDR OPTION_GET(NUMBER,base_addr)
#define IRQ_NUM   OPTION_GET(NUMBER,irq_num)

EMBOX_UNIT_INIT(pl050_mouse_init);


struct pl050_mouse_indev {
	struct input_dev input_dev;
	struct pl050 *pl050_dev;
};

static int pl050_mouse_stop(struct input_dev *dev) {

	/* TODO */
	return 0;
}

static const struct input_dev_ops mouse_input_ops = {
	/*.start = ps_mouse_start,*/
	.stop = pl050_mouse_stop,
};

static struct pl050_mouse_indev mouse_dev = {
	.input_dev = {
		.ops = &mouse_input_ops,
		.name = "ps-mouse",
		.type = INPUT_DEV_MOUSE,
	},
};

static int ps_mouse_event_send(struct input_dev *indev, uint8_t data[3]) {
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

static irq_return_t pl050_mouse_irq_hnd(unsigned int irq_nr, void *data) {
	struct pl050_mouse_indev *pl050_mouse_indev;
	struct pl050 *pl050;
	uint8_t rx_data[3];
	uint32_t tmp;
	struct input_dev *indev;

	pl050_mouse_indev = (struct pl050_mouse_indev *)data;
	pl050 = pl050_mouse_indev->pl050_dev;

	indev = &pl050_mouse_indev->input_dev;

	tmp = pl050->data;
	if (tmp == MOUSE_ACK) {
		goto out;
	}
	rx_data[0] = tmp & 0xFF;

	tmp = pl050->data;
	rx_data[1] = tmp & 0xFF;

	tmp = pl050->data;
	rx_data[2] = tmp & 0xFF;

	ps_mouse_event_send(indev, rx_data);

out:
	return IRQ_HANDLED;
}

static int pl050_mouse_init(void) {
	int res;

	res = irq_attach(IRQ_NUM, pl050_mouse_irq_hnd, 0,
					 &mouse_dev, "ps mouse");
	if (res < 0) {
		return res;
	}

	mouse_dev.pl050_dev = (void*)(uintptr_t)BASE_ADDR;

	pl050_init(mouse_dev.pl050_dev);

	return input_dev_register(&mouse_dev.input_dev);
}

PERIPH_MEMORY_DEFINE(pl050_mouse, BASE_ADDR, 0x1000);
