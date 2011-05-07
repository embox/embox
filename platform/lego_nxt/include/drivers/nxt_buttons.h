/**
 * @file
 *
 * @date 24.10.10
 * @author Anton Kozlov
 */

#ifndef NXT_BUTTONS_H_
#define NXT_BUTTONS_H_

#include <types.h>
#include <drivers/nxt_avr.h>

#define BT_ENTER 1
#define BT_LEFT 2
#define BT_RIGHT 4
#define BT_DOWN 8

typedef uint16_t buttons_t;

/* Buttons pressed at now */
extern uint32_t nxt_buttons_are_pressed(void);

/* Buttons was pressed since last call */
extern uint32_t nxt_buttons_was_pressed(void);

/* Notify driver of possible buttons change */
extern void buttons_updated(buttons_t state);

#endif /* NXT_BUTTONS_H_ */
