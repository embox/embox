/**
 * @file
 *
 * @date Feb 6, 2013
 * @author: Anton Bondarev
 */

#include <stdint.h>
#include <string.h>

#include <drivers/keyboard.h>
#include <drivers/input/input_dev.h>
#include <drivers/input/keymap.h>

#define KEY_MAX (sizeof(key_map) / sizeof(char))

/*
 * Key map
 */
static const uint8_t key_map[] = {
	       0,     0x1b,      '1',      '2',     '3',      '4',      '5',      '6',
	     '7',      '8',      '9',      '0',     '-',      '=',     '\b',     '\t',
	     'q',      'w',      'e',      'r',     't',      'y',      'u',      'i',
	     'o',      'p',      '[',      ']',    '\r', KEY_CTRL,      'a',      's',
	     'd',      'f',      'g',      'h',     'j',      'k',      'l',      ';',
	    '\'',      '`', KEY_SHFT,     '\\',     'z',      'x',      'c',      'v',
	     'b',      'n',      'm',      ',',     '.',      '/', KEY_SHFT,      '*',
	 KEY_ALT,      ' ', KEY_CAPS,   KEY_F1,  KEY_F2,   KEY_F3,   KEY_F4,   KEY_F5,
	  KEY_F6,   KEY_F7,   KEY_F8,   KEY_F9, KEY_F10,        0,        0, KEY_HOME,
	  KEY_UP, KEY_PGUP,        0, KEY_LEFT,       0, KEY_RGHT,        0,  KEY_END,
	KEY_DOWN, KEY_PGDN,  KEY_INS,  KEY_DEL, KEY_F11,  KEY_F12
};

static const uint8_t shift_map[] = {
	       0,     0x1b,     '!',      '@',  '#',      '$',  '%',      '^',
	     '&',      '*',     '(',      ')',  '_',      '+', '\b',     '\t',
	     'Q',      'W',     'E',      'R',  'T',      'Y',  'U',      'I',
	     'O',      'P',     '{',      '}', '\r', KEY_CTRL,  'A',      'S',
	     'D',      'F',     'G',      'H',  'J',      'K',  'L',      ':',
	     '"',      '~',       0,      '|',  'Z',      'X',  'C',      'V',
	     'B',      'N',     'M',      '<',  '>',      '?',    0,      '*',
	 KEY_ALT,      ' ',       0,        0,    0,        0,    0,        0,
	       0,        0,       0,        0,    0,        0,    0, KEY_HOME,
	  KEY_UP, KEY_PGUP,       0, KEY_LEFT,    0, KEY_RGHT,    0,  KEY_END,
	KEY_DOWN, KEY_PGDN, KEY_INS,  KEY_DEL,    0,      0
};

static const unsigned char esc_start[] = {0x1B, 0x5B}; /* esc, '[' */

int keymap_kbd(struct input_event *event) {
	const uint8_t *cur_map;
	uint8_t val;
	int key_index = event->value & KEYBOARD_KEY_MASK;

	if (key_index > KEY_MAX) {
		return -1;
	}

	if (!(event->type & KEY_PRESSED)) {
		return -1;
	}

	cur_map = event->value & SHIFT_PRESSED ? shift_map : key_map;
	val = cur_map[key_index];

	if (event->value & CTRL_PRESSED){
		val = val < 0x40 ? 0 : val & KEYBOARD_CTRL_MASK;
	}

	return val;
}

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



