/**
 * @file
 *
 * @date 24.10.2010
 * @author Anton Kozlov
 */

#ifndef NXT_BUTTONS_H_
#define NXT_BUTTONS_H_

#include <types.h>

#define BT_ENTER 1
#define BT_LEFT 2
#define BT_RIGHT 4
#define BT_DOWN 8

/* Buttons pressed at now */
extern uint32_t nxt_buttons_are_pressed(void);

/* Buttons was pressed since last call */
extern uint32_t nxt_buttons_was_pressed(void);

#endif /* NXT_BUTTONS_H_*/

