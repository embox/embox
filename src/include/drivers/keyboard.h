/**
 * @file
 * @brief
 *
 * @date 31.12.10
 * @author Anton Bondarev
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <drivers/input/input_dev.h>

enum KBD_EVENT_CODE {
	KEY_PRESSED = 0x8000,
};

#define SHIFT_PRESSED 0x10000
#define ALT_PRESSED   0x20000
#define CTRL_PRESSED  0x40000
#define CAPS_PRESSED  0x80000

#define KEYBOARD_KEY_MASK 0xFFFF
#define KEYBOARD_CTRL_MASK 0x1f

#define KEYBOARD_SCAN_CODE_EXT    0xE0

#define KEYBOARD_SCAN_CODE_CTRL   0x1D
#define KEYBOARD_SCAN_CODE_ALT    0x38
#define KEYBOARD_SCAN_CODE_SHIFT  0x2A
#define KEYBOARD_SCAN_CODE_CAPS   0x3A

#endif /* KEYBOARD_H_ */
