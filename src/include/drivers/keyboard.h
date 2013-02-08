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

enum KBD_EVENT_CODE {
	KEY_PRESSED = 0,
};

#endif /* KEYBOARD_H_ */
