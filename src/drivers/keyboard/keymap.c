/**
 * @file
 *
 * @date Feb 6, 2013
 * @author: Anton Bondarev
 */

#include <types.h>
#include <string.h>
#include <drivers/keyboard.h>
#include <drivers/input/input_dev.h>

#if 0
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


const unsigned int keymap[][4] = {
	{0},    /* 0 */
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


#endif


#define KEY_CTRL 0x80
#define KEY_SHFT 0x81
#define KEY_ALT  0x82
#define KEY_CAPS 0x83

#define KEY_INS  0x84
#define KEY_HOME 0x85
#define KEY_END	 0x86
#define KEY_PGUP 0x87
#define KEY_PGDN 0x88
#define KEY_UP   0x89
#define KEY_DOWN 0x8a
#define KEY_LEFT 0x8b
#define KEY_RGHT 0x8c

#define KEY_F1   0x90
#define KEY_F2   0x91
#define KEY_F3   0x92
#define KEY_F4   0x93
#define KEY_F5   0x94
#define KEY_F6   0x95
#define KEY_F7   0x96
#define KEY_F8   0x97
#define KEY_F9   0x98
#define KEY_F10  0x99
#define KEY_F11  0x9a
#define KEY_F12  0x9b


/*
 * Key map
 */
static const uint8_t key_map[] = {
	0,      0x1b,   '1',    '2',    '3',    '4',    '5',    '6',
	'7',    '8',    '9',    '0',    '-',    '=',    '\b',   '\t',
	'q',    'w',    'e',    'r',    't',    'y',    'u',    'i',
	'o',    'p',    '[',    ']',    '\n',   KEY_CTRL, 'a',    's',
	'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';',
	'\'',   '`',    KEY_SHFT, '\\',   'z',    'x',    'c',    'v',
	'b',    'n',    'm',    ',',    '.',    '/',    KEY_SHFT, '*',
	KEY_ALT,  ' ',    KEY_CAPS, KEY_F1,   KEY_F2,   KEY_F3,   KEY_F4,   KEY_F5,
	KEY_F6,   KEY_F7,   KEY_F8,   KEY_F9,   KEY_F10,  0,      0,      KEY_HOME,
	KEY_UP,   KEY_PGUP, 0,      KEY_LEFT, 0,      KEY_RGHT, 0,      KEY_END,
	KEY_DOWN, KEY_PGDN, KEY_INS,  0x7f,   KEY_F11,  KEY_F12
};

#define KEY_MAX (sizeof(key_map) / sizeof(char))

static const uint8_t shift_map[] = {
	0,      0x1b,   '!',    '@',    '#',    '$',    '%',    '^',
	'&',    '*',    '(',    ')',    '_',    '+',    '\b',   '\t',
	'Q',    'W',    'E',    'R',    'T',    'Y',    'U',    'I',
	'O',    'P',    '{',    '}',    '\n',   KEY_CTRL, 'A',    'S',
	'D',    'F',    'G',    'H',    'J',    'K',    'L',    ':',
	'"',    '~',    0,      '|',    'Z',    'X',    'C',    'V',
	'B',    'N',    'M',    '<',    '>',    '?',    0,      '*',
	KEY_ALT,  ' ',    0,      0,      0,      0,      0,      0,
	0,      0,      0,      0,      0,      0,      0,      KEY_HOME,
	KEY_UP,   KEY_PGUP, 0,      KEY_LEFT, 0,      KEY_RGHT, 0,      KEY_END,
	KEY_DOWN, KEY_PGDN, KEY_INS,  0x7f,   0,      0
};

int keymap_kbd(struct input_event *event) {
	if((event->value & 0xFFFF) > KEY_MAX) {
		return -1;
	}

	if(event->value & SHIFT_PRESSED) {
		return (int)shift_map[event->value & 0xFFFF];
	} else {
		return (int)key_map[event->value & 0xFFFF];
	}
}

static const unsigned char esc_start[] = {0x1B, 0x5B}; /* esc, '[' */

int keymap_to_ascii(struct input_event *event, unsigned char ascii_buff[4]) {
	int keycode = keymap_kbd(event);
	int seq_len = 0;

	if(keycode < 0) {
		return 0;
	}

	switch (keycode) {
	case 0:
	case KEY_CAPS:
	case KEY_SHFT:
	case KEY_CTRL:
	case KEY_ALT:
		return 0; /* no ascii symbols */

	case KEY_F1:
	case KEY_F2:
	case KEY_F3:
	case KEY_F4:
	case KEY_F5:
	case KEY_F6:
	case KEY_F7:
	case KEY_F8:
	case KEY_F9:
	case KEY_F10:
	case KEY_F11:
	case KEY_F12:
		return 0; /* no ascii symbols */

	case KEY_INS:
		/*0x7e325b1b */
		seq_len = 4;
		ascii_buff[2] = 0x32;
		ascii_buff[2] = 0x7e;
		break;
	case KEY_HOME:
		/* 0x485b1b */
		seq_len = 3;
		ascii_buff[2] = 0x48;
		break;
	case KEY_END:
		/* 0x465b1b */
		seq_len = 3;
		ascii_buff[2] = 0x46;
		break;
	case KEY_PGUP:
		/* 0x7e355b1b */
		seq_len = 4;
		ascii_buff[2] = 0x35;
		ascii_buff[2] = 0x7e;
		break;
	case KEY_PGDN:
		/* 0x7e365b1b */
		seq_len = 4;
		ascii_buff[2] = 0x36;
		ascii_buff[2] = 0x7e;
		break;
	case KEY_UP:
		/* 0x415b1b */
		seq_len = 3;
		ascii_buff[2] = 0x41;
		break;
	case KEY_DOWN:
		/* 0x425b1b */
		seq_len = 3;
		ascii_buff[2] = 0x42;
		break;
	case KEY_LEFT:
		/* 0x445b1b */
		seq_len = 3;
		ascii_buff[2] = 0x44;
		break;
	case KEY_RGHT:
		/* 0x435b1b */
		seq_len = 3;
		ascii_buff[2] = 0x43;
		break;

	default:
		ascii_buff[0] = (unsigned char)keycode;
		return 1;
	}
	memcpy(ascii_buff, esc_start, sizeof(esc_start));
	return seq_len;
}



