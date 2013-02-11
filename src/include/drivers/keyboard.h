/**
 * @file
 * @brief
 *
 * @date 31.12.10
 * @author Anton Bondarev
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

extern void keyboard_init(void);

extern int keyboard_getc(void);

extern int keyboard_has_symbol(void);
struct input_event;
extern int keymap_kbd(struct input_event *event);

enum KBD_EVENT_CODE {
	KEY_PRESSED = 0x8000,
};



#define SHIFT_PRESSED 0x10000
#define ALT_PRESSED   0x20000
#define CTRL_PRESSED  0x40000
#define CAPS_PRESSED  0x80000

#endif /* KEYBOARD_H_ */
