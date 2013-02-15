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

#define kmc_wait_ibe()	while (inb(I8042_STS_PORT) & I8042_STS_IBF)

static void kmc_send_auxcmd(unsigned char val) {
	kmc_wait_ibe();
	out8(I8042_CMD_PORT, 0x60);
	kmc_wait_ibe();
	out8(I8042_DATA_PORT, val);
}

static int kmc_write_aux(unsigned char val) {
	/* Write the value to the device */
	kmc_wait_ibe();
	outb(I8042_CMD_PORT, 0xd4);
	kmc_wait_ibe();
	outb(I8042_DATA_PORT, val);

	return 0;
}
#include <kernel/printk.h>


static int ps_mouse_get_input_event(void) {
	unsigned char code;

	if ((inb(I8042_STS_PORT) & 0x21) != 0x21) {
		/* this is keyboard scan code */
		return 0;
	}

	code = inb(I8042_DATA_PORT);
	printk("mouse:%X", code);

	return code;
}

static struct input_dev mouse_dev = {
		.name = "PC/2 mouse",
		.irq = 12,
		.getc = ps_mouse_get_input_event
};

static int ps_mouse_init(void) {

	input_dev_register(&mouse_dev);

	kmc_wait_ibe();
	outb(I8042_CMD_PORT, 0xa8); /* Enable aux */

	kmc_write_aux(0xf3); /* Set sample rate */
	kmc_write_aux(100); /* 100 samples/sec */

	kmc_write_aux(0xe8); /* Set resolution */
	kmc_write_aux(3); /* 8 counts per mm */
	kmc_write_aux(0xe7); /* 2:1 scaling */

	kmc_write_aux(0xf4); /* Enable aux device */
	kmc_send_auxcmd(0x47); /* Enable controller ints */


	return 0;
}
