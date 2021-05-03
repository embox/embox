/**
 * @file
 *
 * @date May 1, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_INPUT_MOUSE_PS_MOUSE_H_
#define SRC_DRIVERS_INPUT_MOUSE_PS_MOUSE_H_

#include <stdint.h>

#define MSTAT_BUTMASK   0x07
#define MSTAT_BUTLEFT   0x01
#define MSTAT_BUTRIGHT  0x02
#define MSTAT_BUTMIDDLE 0x04
#define MSTAT_ALONE     0x08
#define MSTAT_XSIGN     0x10
#define MSTAT_YSIGN     0x20
#define MSTAT_XOVER     0x40
#define MSTAT_YOVER     0x80

#define MOUSE_ACK       0xfa

struct input_dev;

extern int ps_mouse_event_send(struct input_dev *indev, uint8_t data[3]);

#endif /* SRC_DRIVERS_INPUT_MOUSE_PS_MOUSE_H_ */
