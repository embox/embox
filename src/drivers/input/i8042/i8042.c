/*
 * @file
 *
 * @date   10.04.2020
 * @author Alexander Kalmuk
 */

#include <embox/unit.h>
#include <drivers/i8042.h>

EMBOX_UNIT_INIT(i8042_init);

int i8042_read_mode(void) {
	i8042_wait_write();
	outb(I8042_CMD_READ_MODE, I8042_CMD_PORT);
	i8042_wait_read();
	return inb(I8042_DATA_PORT);
}

void i8042_write_mode(uint8_t val) {
	i8042_wait_write();
	outb(I8042_CMD_WRITE_MODE, I8042_CMD_PORT);
	i8042_wait_write();
	outb(val, I8042_DATA_PORT);
}

void i8042_write_aux(uint8_t val) {
	i8042_wait_write();
	outb(I8042_CMD_WRITE_MOUSE, I8042_CMD_PORT);
	i8042_wait_write();
	outb(val, I8042_DATA_PORT);
}

static int i8042_init(void) {
	uint8_t mode;

	/* Disable mouse */
	i8042_write_aux(I8042_AUX_DISABLE_DEV);
	outb(I8042_AUX_DISABLE_DEV, I8042_CMD_PORT);

	/* Disable keyboard. */
	i8042_wait_write();
	outb(I8042_CMD_PORT_DIS, I8042_CMD_PORT);
	i8042_wait_write();
	outb(I8042_KBD_RESET_DISABLE, I8042_DATA_PORT);

	/* Disable mouse and keyboard interrupts. */
	mode = i8042_read_mode();
	mode &= ~(I8042_MODE_INTERRUPT | I8042_MODE_MOUSE_INT);
	i8042_write_mode(mode);

	/* Flush keyboard and mouse FIFOs (to clear interrupts if any). */
	while (inb(I8042_STS_PORT) & I8042_STS_OBF) {
		inb(I8042_DATA_PORT);
	}

	return 0;
}
