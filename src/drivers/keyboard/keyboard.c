/**
 * @file
 * @brief Keyboard driver
 *
 * @date 12.12.2010
 * @author Anton Bondarev
 */
#include <types.h>
#include <asm/io.h>

#define I8042_CMD_READ_MODE  0x20
#define I8042_CMD_WRITE_MODE 0x60

#define I8042_MODE_XLATE     0x40


int keyboard_havechar(void) {
	unsigned char c = inb(0x64);
	return (c == 0xFF) ? 0 : c & 1;
}

unsigned char keyboard_get_scancode(void) {
	unsigned char ch = 0;

	if (keyboard_havechar())
		ch = inb(0x60);

	return ch;
}

int keyboard_getchar(void) {
	unsigned char ch;
	int ret = 0;

	while (!keyboard_havechar())
		;

	ch = keyboard_get_scancode();

	return ret;
}

static int keyboard_wait_read(void) {
	while (0 == (inb(0x64) & 0x01))
		;

	return 0;
}

static int keyboard_wait_write(void) {
	while (0 != (inb(0x64) & 0x02))
		;

	return 0;
}

static unsigned char keyboard_get_mode(void) {
	outb(I8042_CMD_READ_MODE, 0x64);
	keyboard_wait_read();
	return inb(0x60);
}

static void keyboard_set_mode(unsigned char mode) {
	outb(I8042_CMD_WRITE_MODE, 0x64);
	keyboard_wait_write();
	outb(mode, 0x60);
}

void keyboard_init(void) {
	uint8_t mode;

	/* If 0x64 returns 0xff, then we have no keyboard
	 * controller */

	if (inb(0x64) == 0xFF)
		return;

	/* Empty keyboard buffer */
	while (keyboard_havechar())
		keyboard_getchar();

	/* Read the current mode */
	mode = keyboard_get_mode();

	/* Turn on scancode translate mode so that we can
	 use the scancode set 1 tables */

	mode |= I8042_MODE_XLATE;

	/* Write the new mode */
	keyboard_set_mode(mode);
}
