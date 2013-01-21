/**
 * @file
 * @brief Keyboard driver
 *
 * @date 12.12.10
 * @author Anton Bondarev
 */

#include <types.h>
#include <asm/io.h>
#include <drivers/keyboard.h>
#include <kernel/irq.h>
#include <kernel/printk.h>

#define  I8042_CMD_READ_MODE   0x20
#define  I8042_CMD_WRITE_MODE  0x60
#define  I8042_CMD_PORT_DIS    0xAD
#define  I8042_CMD_PORT_EN     0xAE

#define  I8042_MODE_XLATE      0x40
#define  I8042_MODE_DISABLE    0x10


#define  KEY_CURSOR_UP         0xb8
#define  KEY_CURSOR_DOWN       0xb2
#define  KEY_CURSOR_LEFT       0xb4
#define  KEY_CURSOR_RIGHT      0xb6
#define  KEY_CURSOR_HOME       0xb7
#define  KEY_CURSOR_END        0xb1
#define  KEY_PAGE_UP           0xb9
#define  KEY_PAGE_DOWN         0xb3
#define  KEY_INSERT            0xb0
#define  KEY_DELETE            0xae
#define  KEY_F1                0xc0

#define  CMD_PORT              0x64
#define  STATUS_PORT           0x64
#define  DATA_PORT             0x60

#define  IRQ1 1

#define keyboard_wait_read()  do {} while (0 == (inb(STATUS_PORT) & 0x01))
#define keyboard_wait_write() do {} while (0 != (inb(STATUS_PORT) & 0x02))

static const unsigned int keymap[][4] = {
	{0       },    /* 0 */
	{27,	27 },  /* 1 - ESC */
	{'1',	'!'},  /* 2 */
	{'2',	'@'},
	{'3',	'#'},
	{'4',	'$'},
	{'5',	'%'},
	{'6',	'^'},
	{'7',	'&'},
	{'8',	'*'},
	{'9',	'('},
	{'0',	')'},
	{'-',	'_'},
	{'=',	'+'},
	{8,	8  },  /* 14 - Backspace */
	{'\t', '\t'},  /* 15 */
	{'q',	'Q'},
	{'w',	'W'},
	{'e',	'E'},
	{'r',	'R'},
	{'t',	'T'},
	{'y',	'Y'},
	{'u',	'U'},
	{'i',	'I'},
	{'o',	'O'},
	{'p',	'P'},
	{'[',	'{'},
	{']','}'},     /* 27 */
	{'\r','\r'},   /* 28 - Enter */
	{0,	0  },  /* 29 - Ctrl */
	{'a',	'A'},  /* 30 */
	{'s',	'S'},
	{'d',	'D'},
	{'f',	'F'},
	{'g',	'G'},
	{'h',	'H'},
	{'j',	'J'},
	{'k',	'K'},
	{'l',	'L'},
	{';',	':'},
	{'\'',  '"'},  /* 40 */
	{'`',	'~'},  /* 41 */
	{0xff, 0xff},  /* 42 - Left Shift */
	{'\\',  '|'},  /* 43 */
	{'z',	'Z'},  /* 44 */
	{'x',	'X'},
	{'c',	'C'},
	{'v',	'V'},
	{'b',	'B'},
	{'n',	'N'},
	{'m',	'M'},
	{',',	'<'},
	{'.',	'>'},
	{'/',   '?'},  /* 53 */
	{0xff, 0xff},  /* 54 - Right Shift */
	{0,	 0},   /* 55 - Print Screen */
	{0,	 0},   /* 56 - Alt */
	{' ',  ' '},   /* 57 - Space bar */
	{0,	 0},   /* 58 - Caps Lock */
	{0,	 0},   /* 59 - F1 */
	{0,	 0},   /* 60 - F2 */
	{0,	 0},   /* 61 - F3 */
	{0,	 0},   /* 62 - F4 */
	{0,	 0},   /* 63 - F5 */
	{0,	 0},   /* 64 - F6 */
	{0,	 0},   /* 65 - F7 */
	{0,	 0},   /* 66 - F8 */
	{0,	 0},   /* 67 - F9 */
	{0,	 0},   /* 68 - F10 */
	{0,	 0},   /* 69 - Num Lock */
	{0,	 0},   /* 70 - Scroll Lock */
	{0xb7,0xb7,0x485b1b},   /* 71 - Numeric keypad 7, Home */
	{0xb8,0xb8,0x415b1b},   /* 72 - Numeric keypad 8, Up Arrow */
	{0xb9,0xb9,0x7e355b1b},   /* 73 - Numeric keypad 9, Page up */
	{'-',  '-'},   /* 74 - Numeric keypad '-' */
	{0xb4,0xb4,0x445b1b},   /* 75 - Numeric keypad 4, Left Arrow*/
	{0xb5,0xb5},   /* 76 - Numeric keypad 5 */
	{0xb6,0xb6,0x435b1b},   /* 77 - Numeric keypad 6, Right Arrow*/
	{'+',  '+'},   /* 78 - Numeric keypad '+' */
	{0xb1,0xb1,0x465b1b},   /* 79 - Numeric keypad 1, End*/
	{0xb2,0xb2,0x425b1b},   /* 80 - Numeric keypad 2, Down Arrow */
	{0xb3,0xb3,0x7e365b1b},   /* 81 - Numeric keypad 3, Page Down*/
	{0xb0,0xb0,0x7e325b1b},   /* 82 - Numeric keypad 0, Insert*/
	{0xae,0xae,0x7e335b1b},   /* 83 - Numeric keypad '.', Delete */
};

static int keyboard_havechar(void) {
	unsigned char c = inb(STATUS_PORT);
	return (c == 0xFF) ? 0 : c & 1;
}

static void keyboard_send_cmd(uint8_t cmd) {
	keyboard_wait_write();
	outb(cmd, CMD_PORT);
}

static unsigned char keyboard_get_mode(void) {
	keyboard_send_cmd(I8042_CMD_READ_MODE);
	keyboard_wait_read();
	return inb(DATA_PORT);
}

static void keyboard_set_mode(unsigned char mode) {
	keyboard_send_cmd(I8042_CMD_WRITE_MODE);
	keyboard_wait_write();
	outb(mode, DATA_PORT);
}

int keyboard_has_symbol(void) {
	return keyboard_havechar();
}

char keyboard_getc(void) {
	static unsigned int shift_state;
	static unsigned int outp;
	uint8_t status, scan_code;
	unsigned char extended_state;

	while (1) {
		if (outp) {
			unsigned char ch = outp & 0xff;
			outp >>=8;
			return ch;
		}

		status = inb(CMD_PORT);
		if ((status & 0x01) == 0) {
			continue;
		}
		scan_code = inb(DATA_PORT);
		if ((status & 0x20) != 0) {
			continue;
		}

		if (scan_code == 0xe0) {
			extended_state = 2;
			continue;
		}

		if (scan_code & 0x80) {
			scan_code &= 0x7f;
			if (keymap[scan_code][0] == 0xff) {
				shift_state = 0;
			}
			extended_state = 0;
			continue;
		}

		if ((scan_code & 0x7f) >= sizeof(keymap)/sizeof(keymap[0])) {
			continue;
		}


		outp = keymap[scan_code][extended_state ? extended_state : shift_state];

		extended_state = 0;

		if (outp == 0xff) {
			shift_state = 1;
			outp = 0;
		}
	}
}

#if 0

#include <kernel/printk.h>

static irq_return_t kbd_handler(unsigned int irq_nr, void *data) {
	uint8_t scancode;
	scancode = in8(DATA_PORT);
	//TODO:
	printk("keycode 0x%X\n", scancode);
	return IRQ_HANDLED;
}
#endif
void keyboard_init(void) {
	uint8_t mode;

	/* If 0x64 returns 0xff, then we have no keyboard
	 * controller */
	if (inb(0x64) == 0xFF) {
		return;
	}

	keyboard_send_cmd(I8042_CMD_PORT_DIS);

	/* Empty keyboard buffer */
	while (keyboard_havechar()) keyboard_getc();

	/* Read the current mode */
	mode = keyboard_get_mode();
	/* Turn on scancode translate mode so that we can
	 use the scancode set 1 tables */
	mode |= I8042_MODE_XLATE;
	/* Enable keyboard. */
	mode &= ~I8042_MODE_DISABLE;
	/* Write the new mode */
	keyboard_set_mode(mode);
	printk("Keyboard init OK! \n");

	//irq_attach(IRQ1, kbd_handler, 0, NULL, "kbd");

	keyboard_send_cmd(I8042_CMD_PORT_EN);
}
