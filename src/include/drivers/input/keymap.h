/*
 * @file
 *
 * @date Feb 18, 2013
 * @author: Anton Bondarev
 */

#ifndef KEYMAP_H_
#define KEYMAP_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#define KEY_CTRL 0x80
#define KEY_SHFT 0x81
#define KEY_ALT  0x82
#define KEY_CAPS 0x83

#define KEY_INS  0x84
#define KEY_DEL  0x85
#define KEY_HOME 0x86
#define KEY_END	 0x87
#define KEY_PGUP 0x88
#define KEY_PGDN 0x89
#define KEY_UP   0x8a
#define KEY_DOWN 0x8b
#define KEY_LEFT 0x8c
#define KEY_RGHT 0x8d

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

struct input_event;

extern int keymap_to_ascii(struct input_event *event, unsigned char ascii_buff[4]);

extern int keymap_kbd(struct input_event *event);

__END_DECLS

#endif /* KEYMAP_H_ */
