/*
 * @file
 *
 * @date Feb 14, 2013
 * @author: Anton Bondarev
 */
#include <asm/io.h>
#include <embox/unit.h>
#include <drivers/input/input_dev.h>

#include <drivers/i8042.h>

EMBOX_UNIT_INIT(ps_mouse_init);


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
static int ps_mouse_get_input_event(void) {
	unsigned char code;

	if ((inb(I8042_STS_PORT) & 0x21) != 0x21) {
		/* this is keyboard scan code */
		return 0;
	}

	code = inb(I8042_DATA_PORT);

	return code;
}

static struct input_dev mouse_dev = {
		.name = "mouse",
		.irq = 12,
		.getc = ps_mouse_get_input_event
};

static int ps_mouse_init(void) {

	input_dev_register(&mouse_dev);

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
