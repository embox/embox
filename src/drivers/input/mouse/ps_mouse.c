/*
 * @file
 *
 * @date Feb 14, 2013
 * @author: Anton Bondarev
 */

#include <asm/io.h>
#include <errno.h>
#include <embox/unit.h>
#include <drivers/input/input_dev.h>

#include <drivers/i8042.h>

EMBOX_UNIT_INIT(ps_mouse_init);

#define MSTAT_BUTMASK 0x07
#define MSTAT_ALONE   0x08
#define MSTAT_XSIGN   0x10
#define MSTAT_YSIGN   0x20
#define MSTAT_XOVER   0x40
#define MSTAT_YOVER   0x80

#define MOUSE_ACK     0xfa

struct ps2_mouse_indev {
	struct input_dev input_dev;
};

static void kmc_send_auxcmd(unsigned char val) {
	kmc_wait_ibe();
	out8(0x60, I8042_CMD_PORT);
	kmc_wait_ibe();
	out8(val, I8042_DATA_PORT);
}

static int kmc_write_aux(unsigned char val) {
	/* Write the value to the device */
	kmc_wait_ibe();
	outb(0xd4, I8042_CMD_PORT);
	kmc_wait_ibe();
	outb(val, I8042_DATA_PORT);

	return 0;
}

//http://lists.gnu.org/archive/html/qemu-devel/2004-11/msg00082.html
static int ps_mouse_get_input_event(struct input_dev *dev, struct input_event *ev) {
	unsigned char data;

	if ((inb(I8042_STS_PORT) & 0x21) != 0x21) {
		/* this is keyboard scan code */
		return -EAGAIN;
	}

	data = inb(I8042_DATA_PORT);
	if (data == MOUSE_ACK) {
		return -EAGAIN;
	}

	ev->type = data;

	data = inb(I8042_DATA_PORT);
	ev->value = ((ev->type & MSTAT_XSIGN ? 0xff00 : 0) | data) << 16;
	data = inb(I8042_DATA_PORT);
	ev->value |= (ev->type & MSTAT_YSIGN ? 0xff00 : 0) | data;

	ev->type  &= MSTAT_BUTMASK;

	return 0;
}

static int ps_mouse_start(struct input_dev *dev) {
	kmc_wait_ibe();
	outb(0xa8, I8042_CMD_PORT); /* Enable aux */

	kmc_write_aux(0xf3); /* Set sample rate */
	kmc_write_aux(100); /* 100 samples/sec */

	kmc_write_aux(0xe8); /* Set resolution */
	kmc_write_aux(3); /* 8 counts per mm */
	kmc_write_aux(0xe7); /* 2:1 scaling */

	kmc_write_aux(0xf4); /* Enable aux device */
	kmc_send_auxcmd(0x47); /* Enable controller ints */

	return 0;
}

static int ps_mouse_stop(struct input_dev *dev) {

	/* TODO */
	return 0;
}

static const struct input_dev_ops ps_mouse_input_ops = {
	/*.start = ps_mouse_start,*/
	.stop = ps_mouse_stop,
	.event_get = ps_mouse_get_input_event,
};

static struct ps2_mouse_indev mouse_dev = {
	.input_dev = {
		.ops = &ps_mouse_input_ops,
		.name = "mouse",
		.type = INPUT_DEV_MOUSE,
		.irq = 12,
	},
};

static int ps_mouse_init(void) {

	ps_mouse_start(NULL);

	return input_dev_register(&mouse_dev.input_dev);
}
