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
#define MSTAT_XSIGN   0x10
#define MSTAT_YSIGN   0x20
#define MSTAT_XOVER   0x40
#define MSTAT_YOVER   0x80

struct ps2_mouse_indev {
	struct input_dev input_dev;
	char byteseq_state;
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
	struct ps2_mouse_indev *ps2mouse = (struct ps2_mouse_indev *) dev;
	unsigned char data;
	int ret;

	if ((inb(I8042_STS_PORT) & 0x21) != 0x21) {
		/* this is keyboard scan code */
		ret = -EAGAIN;
	}

	data = inb(I8042_DATA_PORT);

	switch(ps2mouse->byteseq_state) {
	case 0:
		ev->type = data;
		ret = -EAGAIN;
		break;
	case 1:
		ev->value = (ev->type & MSTAT_XSIGN ? 0xff00 : 0) | data;
		ret = -EAGAIN;
		break;
	case 2:
		ev->value <<= 16;
	       	ev->value |= (ev->type & MSTAT_YSIGN ? 0xff00 : 0) | data;
		ev->type  &= MSTAT_BUTMASK;
		ret = 0;
		break;
	}

	ps2mouse->byteseq_state = (ps2mouse->byteseq_state + 1) % 3;

	return ret;
}

static struct ps2_mouse_indev mouse_dev = {
	.input_dev = {
		.name = "mouse",
		.type = INPUT_DEV_MOUSE,
		.irq = 12,
		.event_get = ps_mouse_get_input_event,
	},
};

static int ps_mouse_init(void) {

	input_dev_register(&mouse_dev.input_dev);

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
