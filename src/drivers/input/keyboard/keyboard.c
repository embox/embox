/**
 * @file
 * @brief Keyboard driver
 *
 * @date 12.12.10
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stdint.h>

#include <drivers/keyboard.h>
#include <drivers/input/keymap.h>
#include <drivers/input/input_dev.h>
#include <drivers/indev_manager.h>
#include <drivers/iodev.h>
#include <embox/unit.h>
#include <drivers/diag.h>
#include <drivers/tty.h>
#include <drivers/video_term.h>
#include <drivers/i8042.h>

#define REGISTER_IN_VT OPTION_GET(NUMBER,register_in_vt)

EMBOX_UNIT_INIT(keyboard_init);

int key_is_pressed(struct input_event *event) {
	return event->type & KEY_PRESSED;
}

static void keyboard_send_cmd(uint8_t cmd) {
	unsigned int status;
	keyboard_wait_write(status);
	outb(cmd, I8042_CMD_PORT);
}

static unsigned char keyboard_get_mode(void) {
	unsigned char status;

	keyboard_send_cmd(I8042_CMD_READ_MODE);
	keyboard_wait_read(status);
	return inb(I8042_DATA_PORT);
}

static void keyboard_set_mode(unsigned char mode) {
	unsigned char status;

	keyboard_send_cmd(I8042_CMD_WRITE_MODE);
	keyboard_wait_write(status);
	outb(mode, I8042_DATA_PORT);
}

static int kbd_state;

static void kbd_key_serv_press(int state, int flag) {
	if (state & KEY_PRESSED) {
		kbd_state |= flag;
	} else {
		kbd_state &= ~flag;
	}
}

static int keyboard_get_input_event(struct input_dev *dev, struct input_event *event) {
	uint8_t scan_code, status;
	int flag = 0;

	event->type = event->value = 0;

	status = keyboard_read_stat();

	if (!(status & I8042_STS_OBF)) {
		return -EAGAIN;
	}

	scan_code = inb(I8042_DATA_PORT);

	if (scan_code == KEYBOARD_SCAN_CODE_EXT) {
		return -EAGAIN;
	}

	if (status & I8042_STS_AUXOBF) {
		return -EAGAIN;
	}

	if (scan_code & 0x80) {
		/* key unpressed */
		event->type &= ~KEY_PRESSED;
	} else {
		/* key pressed */
		event->type |= KEY_PRESSED;
	}
	scan_code &= 0x7F;

	switch(scan_code) {
	case KEYBOARD_SCAN_CODE_CTRL:
		flag = CTRL_PRESSED;
		break;
	case KEYBOARD_SCAN_CODE_ALT:
		flag = ALT_PRESSED;
		break;
	case KEYBOARD_SCAN_CODE_SHIFT:
		flag = SHIFT_PRESSED;
		break;
	case KEYBOARD_SCAN_CODE_CAPS:
		flag = CAPS_PRESSED;
		break;
	}
	kbd_key_serv_press(event->type, flag);

	event->value = kbd_state | scan_code;

	return 0;
}
static struct input_dev kbd_dev = {
		.name = "keyboard",
		.type = INPUT_DEV_KBD,
		.irq = 1,
		.event_get = keyboard_get_input_event,
};

#if REGISTER_IN_VT
void indev_manager_init_report(const char *name){
	vterm_open_indev(&diag_vterm, name);
}

static char kbd_getc(void) {
	char ch;
	tty_read(&diag_vterm.tty, &ch, 1);
	return ch;
}


static const struct iodev_ops iodev_diag_ops_struct = {
	.getc = kbd_getc,//&diag_getc,
	.putc = &diag_putc,
	.kbhit = NULL,
};

static void register_diag_input(void) {
	iodev_setup(&iodev_diag_ops_struct);
}

#endif

static int keyboard_init(void) {
	uint8_t mode;

	/* If 0x64 returns 0xff, then we have no keyboard
	 * controller */
	if (inb(0x64) == 0xFF) {
		return 0;
	}

	keyboard_send_cmd(I8042_CMD_PORT_DIS);

	while (keyboard_havechar()) inb(I8042_DATA_PORT);

	/* Read the current mode */
	mode = keyboard_get_mode();
	/* Turn on scancode translate mode so that we can
	 use the scancode set 1 tables */
	mode |= I8042_MODE_XLATE;
	/* Enable keyboard. */
	mode &= ~I8042_MODE_DISABLE;
	/* Enable interrupt */
	mode |= I8042_MODE_INTERRUPT;
	/* Write the new mode */
	keyboard_set_mode(mode);

	keyboard_send_cmd(I8042_CMD_PORT_EN);

	input_dev_register(&kbd_dev);
	kbd_state = 0;

#if REGISTER_IN_VT
	indev_manager_init_report(kbd_dev.name);

	register_diag_input();
#endif

	return 0;
}
