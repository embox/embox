/**
 * @file
 *
 * @date 24.10.2010
 * @author Anton Kozlov
 */

#include <types.h>

#ifndef NXT_BUTTONS_H_
#define NXT_BUTTONS_H_

#define BT_ENTER 1
#define BT_LEFT 2
#define BT_RIGHT 4
#define BT_DOWN 8

/* Buttons pressed at now */
uint32_t nxt_buttons_are_pressed(void);

/* Buttons was pressed since last call */
uint32_t nxt_buttons_was_pressed(void);
#endif /* NXT_BUTTONS_H_*/
