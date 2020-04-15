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
#include <kernel/irq.h>

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

//http://lists.gnu.org/archive/html/qemu-devel/2004-11/msg00082.html
static int ps_mouse_get_input_event(struct input_dev *dev, struct input_event *ev) {
	uint8_t data;
	int ret = 0;

	irq_lock();
	{
		data = inb(I8042_STS_PORT);

		if ((data & (I8042_STS_AUXOBF | I8042_STS_OBF))
				!= (I8042_STS_AUXOBF | I8042_STS_OBF)) {
			/* this is keyboard scan code */
			ret = -EAGAIN;
			goto out;
		}

		data = inb(I8042_DATA_PORT);
		if (data == MOUSE_ACK) {
			ret = -EAGAIN;
			goto out;
		}

		ev->type = data;

		data = inb(I8042_DATA_PORT);
		ev->value = ((ev->type & MSTAT_XSIGN ? 0xff00 : 0) | data) << 16;
		data = inb(I8042_DATA_PORT);
		ev->value |= (ev->type & MSTAT_YSIGN ? 0xff00 : 0) | data;

		ev->type  &= MSTAT_BUTMASK;
	}
out:
	irq_unlock();
	return ret;
}

static int ps_mouse_start(struct input_dev *dev) {
	uint8_t mode;

	irq_lock();
	{
		mode = i8042_read_mode();
		mode |= I8042_MODE_XLATE | I8042_MODE_SYS | I8042_MODE_MOUSE_INT;
		mode &= ~I8042_MODE_DISABLE_MOUSE;
		i8042_write_mode(mode);

		i8042_wait_write();
		outb(I8042_CMD_MOUSE_ENABLE, I8042_CMD_PORT);

		i8042_write_aux(I8042_AUX_SET_SAMPLE);
		i8042_write_aux(100); /* 100 samples/sec */

		i8042_write_aux(I8042_AUX_SET_RES);
		i8042_write_aux(3); /* 8 counts per mm */
		i8042_write_aux(I8042_AUX_SET_SCALE21);

		i8042_write_aux(I8042_AUX_ENABLE_DEV);
	}
	irq_unlock();

	return 0;
}

static int ps_mouse_stop(struct input_dev *dev) {

	/* TODO */
	return 0;
}

static const struct input_dev_ops ps_mouse_input_ops = {
	/*.start = ps_mouse_start,*/
	.stop = ps_mouse_stop,
};

static struct ps2_mouse_indev mouse_dev = {
	.input_dev = {
		.ops = &ps_mouse_input_ops,
		.name = "ps-mouse",
		.type = INPUT_DEV_MOUSE,
	},
};

static irq_return_t ps_mouse_irq_hnd(unsigned int irq_nr, void *data) {
	struct input_dev *dev = (struct input_dev *) data;
	struct input_event ev;
	int ret;

	ret = ps_mouse_get_input_event(dev, &ev);
	if (!ret && dev->event_cb) {
		input_dev_report_event(dev, &ev);
	}

	return IRQ_HANDLED;
}

static int ps_mouse_init(void) {
	int res;

	res = irq_attach(I8042_MOUSE_IRQ, ps_mouse_irq_hnd, 0,
					 &mouse_dev.input_dev, "ps mouse");
	if (res < 0) {
		return res;
	}

	ps_mouse_start(NULL);

	return input_dev_register(&mouse_dev.input_dev);
}
