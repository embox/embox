/**
 * @file
 * @brief Interface for on-brick buttons of NXT brick
 *
 * @date 24.10.10
 * @author Anton Kozlov
 */

#ifndef NXT_BUTTONS_H_
#define NXT_BUTTONS_H_

#include <types.h>

#define NXT_BOTTON_ENTER  1
#define NXT_BUTTON_LEFT   2
#define NXT_BOTTON_RIGHT  4
#define NXT_BOTTON_DOWN   8

/**
 * Buttons mask. Represents several buttons in time. E.g.
 * simultaneous pressing NXT_BOTTON_ENTER and NXT_BOTTON_DOWN gives '9' value
 * (equals '1 | 8')
 */
typedef uint16_t nxt_buttons_mask_t;

/**
 * Gives buttons mask of currently pushed down buttons
 * @return mask of currently pushed buttons
 */
extern nxt_buttons_mask_t nxt_buttons_pushed(void);

/**
 * Gives buttons mask of pressed buttons since last call of
 * #nxt_buttons_pressed
 * @return mask of pressed buttons since last call
 */
extern nxt_buttons_mask_t nxt_buttons_pressed(void);

#endif /* NXT_BUTTONS_H_ */
