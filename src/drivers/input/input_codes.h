/**
 * @file
 * @brief
 *
 * @date 26.04.20
 * @author Alexander Kalmuk
 */

#ifndef DRIVERS_INPUT_INPUT_CODES_H_
#define DRIVERS_INPUT_INPUT_CODES_H_

/* Keyboard */
#define KBD_KEY_PRESSED   0x8000


#define KBD_SHIFT_PRESSED 0x10000
#define KBD_ALT_PRESSED   0x20000
#define KBD_CTRL_PRESSED  0x40000
#define KBD_CAPS_PRESSED  0x80000

#define KEYBOARD_KEY_MASK 0xFFFF
#define KEYBOARD_CTRL_MASK 0x1f

#define KEYBOARD_SCAN_CODE_EXT    0xE0

#define KEYBOARD_SCAN_CODE_CTRL   0x1D
#define KEYBOARD_SCAN_CODE_ALT    0x38
#define KEYBOARD_SCAN_CODE_SHIFT  0x2A
#define KEYBOARD_SCAN_CODE_CAPS   0x3A


/* Touchscreen */

#define TS_TOUCH_1              1
#define TS_TOUCH_2              2
#define TS_TOUCH_1_RELEASED     3
#define TS_TOUCH_2_RELEASED     4
#define TS_TOUCH_PRESSURE       5
/* TODO Should we move it to the common part input_dev.h? */
#define TS_EVENT_NEXT           (1 << 31)

/* Mouse */
#define MOUSE_BUTTON_PRESSED    0x7
#define MOUSE_BUTTON_LEFT       0x1
#define MOUSE_BUTTON_RIGHT      0x2
#define MOUSE_BUTTON_MIDDLE     0x4

#endif /* DRIVERS_INPUT_INPUT_CODES_H_ */
