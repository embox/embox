/**
 * @file
 *
 * @date May 1, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_INPUT_KEYBOARD_PS_KEYBOARD_H_
#define SRC_DRIVERS_INPUT_KEYBOARD_PS_KEYBOARD_H_

#include <stdint.h>

struct input_event;

extern int keyboard_scan_code_to_event(uint8_t scan_code, struct input_event *event);

#endif /* SRC_DRIVERS_INPUT_KEYBOARD_PS_KEYBOARD_H_ */
