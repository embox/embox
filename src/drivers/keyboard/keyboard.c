/**
 * @file
 * @brief Keyboard driver
 *
 * @date 12.12.2010
 * @author Anton Bondarev
 */
#include <types.h>
#include <asm/io.h>
#include <drivers/keyboard.h>
#include <kernel/irq.h>

#define  I8042_CMD_READ_MODE   0x20
#define  I8042_CMD_WRITE_MODE  0x60

#define  I8042_MODE_XLATE      0x40

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
#define  DATA_PORT             0x60

#define  IRQ1 1

static const unsigned char keymap[][2] = {
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
	{0xb7,0xb7},   /* 71 - Numeric keypad 7 */
	{0xb8,0xb8},   /* 72 - Numeric keypad 8 */
	{0xb9,0xb9},   /* 73 - Numeric keypad 9 */
	{'-',  '-'},   /* 74 - Numeric keypad '-' */
	{0xb4,0xb4},   /* 75 - Numeric keypad 4 */
	{0xb5,0xb5},   /* 76 - Numeric keypad 5 */
	{0xb6,0xb6},   /* 77 - Numeric keypad 6 */
	{'+',  '+'},   /* 78 - Numeric keypad '+' */
	{0xb1,0xb1},   /* 79 - Numeric keypad 1 */
	{0xb2,0xb2},   /* 80 - Numeric keypad 2 */
	{0xb3,0xb3},   /* 81 - Numeric keypad 3 */
	{0xb0,0xb0},   /* 82 - Numeric keypad 0 */
	{0xae,0xae},   /* 83 - Numeric keypad '.' */
};

int keyboard_get_scancode(void) {
	static unsigned shift_state;
	unsigned status, scan_code, ch;
	status = inb(CMD_PORT);
	scan_code = inb(DATA_PORT);
	if ((status & 0x20) != 0) {
		return -1;
	}
	if ((scan_code & 0x7f) >= sizeof(keymap) / sizeof(keymap[0])) {
		return -1;
	}
	if (scan_code & 0x80) {
		scan_code &= 0x7f;
		if (keymap[scan_code][0] == 0xff) {
			shift_state = 0;
		}
		return -1;
	}
	ch = keymap[scan_code][shift_state];
	if (ch == (unsigned) 0xff) {
		shift_state = 1;
		return -1;
	}
	if (ch == 0) {
		return -1;
	}
	return ch;
}

int keyboard_getchar(void) {
	static unsigned shift_state;
	unsigned status, scan_code, ch;

	while (1) {
		status = inb(CMD_PORT);
		if ((status & 0x01) == 0) {
			continue;
		}
		scan_code = inb(DATA_PORT);
		if ((status & 0x20) != 0) {
			continue;
		}
		if ((scan_code & 0x7f) >= sizeof(keymap)/sizeof(keymap[0])) {
			continue;
		}
		if (scan_code & 0x80) {
			scan_code &= 0x7f;
			if (keymap[scan_code][0] == 0xff) {
				shift_state = 0;
			}
			continue;
		}
		ch = keymap[scan_code][shift_state];
		if (ch == (unsigned) 0xff) {
			shift_state = 1;
			continue;
		}
		if (ch == 0) {
			continue;
		}
		return ch;
	}
}

static int keyboard_havechar(void) {
	unsigned char c = inb(0x64);
	return (c == 0xFF) ? 0 : c & 1;
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
	outb(I8042_CMD_READ_MODE, CMD_PORT);
	keyboard_wait_read();
	return inb(DATA_PORT);
}

static void keyboard_set_mode(unsigned char mode) {
	outb(I8042_CMD_WRITE_MODE, CMD_PORT);
	keyboard_wait_write();
	outb(mode, DATA_PORT);
}
#if 0
static irq_return_t kbd_handler(irq_nr_t irq_nr, void *data) {
	uint8_t scancode;
	scancode = in8(DATA_PORT);
	//TODO:
	TRACE("keycode 0x%X\n", scancode);
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

	/* Empty keyboard buffer */
	while (keyboard_havechar()) keyboard_getchar();

	/* Read the current mode */
	mode = keyboard_get_mode();

	/* Turn on scancode translate mode so that we can
	 use the scancode set 1 tables */
	mode |= I8042_MODE_XLATE;

	/* Write the new mode */
	keyboard_set_mode(mode);

	//irq_attach(IRQ1, kbd_handler, 0, NULL, "kbd");
}
