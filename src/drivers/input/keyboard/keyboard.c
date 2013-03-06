/**
 * @file
 * @brief Keyboard driver
 *
 * @date 12.12.10
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <drivers/diag.h>

#include <asm/io.h>

#include <drivers/keyboard.h>
#include <drivers/input/keymap.h>
#include <drivers/input/input_dev.h>
#include <embox/unit.h>

#include <drivers/i8042.h>

EMBOX_UNIT_INIT(keyboard_init);

static int keyboard_havechar(void) {
	unsigned char c = inb(I8042_STS_PORT);
	return (c == 0xFF) ? 0 : c & 1;
}

static void keyboard_send_cmd(uint8_t cmd) {
	keyboard_wait_write();
	outb(cmd, I8042_CMD_PORT);
}

static unsigned char keyboard_get_mode(void) {
	keyboard_send_cmd(I8042_CMD_READ_MODE);
	keyboard_wait_read();
	return inb(I8042_DATA_PORT);
}

static void keyboard_set_mode(unsigned char mode) {
	keyboard_send_cmd(I8042_CMD_WRITE_MODE);
	keyboard_wait_write();
	outb(mode, I8042_DATA_PORT);
}

static int kbd_state;

int key_is_pressed(struct input_event *event) {
	return event->type & KEY_PRESSED;
}

static void kbd_key_serv_press(int state, int flag) {
	if(state & KEY_PRESSED) {
		kbd_state |= flag;
	} else {
		kbd_state &= ~flag;
	}
}

static int keyboard_get_input_event(struct input_dev *dev, struct input_event *event) {
	uint8_t scan_code;
	int flag = 0;
	keyboard_wait_read();

	scan_code = inb(I8042_DATA_PORT);

	if(scan_code == KEYBOARD_SCAN_CODE_EXT) {
		keyboard_wait_read();
		scan_code = inb(I8042_DATA_PORT);
	}
	if(scan_code & 0x80) {
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
		.indev_get = keyboard_get_input_event,
};

static int keyboard_getc(void) {
	struct input_dev *kbd = &kbd_dev;
	static unsigned char ascii_buff[4];
	static int ascii_len;
	static int seq_cnt = 0;
	struct input_event event;

	if(ascii_len > seq_cnt) {
		return ascii_buff[seq_cnt++];
	}
	ascii_len = 0;

	do {
		input_dev_event(kbd, &event);

		if(key_is_pressed(&event)) {
			ascii_len = keymap_to_ascii(&event, ascii_buff);
		}

	} while(ascii_len == 0);

	seq_cnt = 0;

	return ascii_buff[seq_cnt++];
}

static int keyboard_init(void) {
	uint8_t mode;

	/* If 0x64 returns 0xff, then we have no keyboard
	 * controller */
	if (inb(0x64) == 0xFF) {
		return 0;
	}

	keyboard_send_cmd(I8042_CMD_PORT_DIS);

	/* Empty keyboard buffer */
	while (keyboard_havechar()) inb(I8042_DATA_PORT);

	/* Read the current mode */
	mode = keyboard_get_mode();
	/* Turn on scancode translate mode so that we can
	 use the scancode set 1 tables */
	mode |= I8042_MODE_XLATE;
	/* Enable keyboard. */
	mode &= ~I8042_MODE_DISABLE;
	/* Write the new mode */
	keyboard_set_mode(mode);

	keyboard_send_cmd(I8042_CMD_PORT_EN);

	input_dev_register(&kbd_dev);
	kbd_state = 0;

	return 0;
}

/*
 * Diag interface
 */
char diag_getc(void) {
	return keyboard_getc();
}

int diag_kbhit(void) {
	return keyboard_havechar();
}
