/**
 * @file
 * @brief Interface for controlling 2 leds on Olimex Debug board
 *
 * @date 26.09.2010
 * @author Anton Kozlov
 */

#ifndef _AT91_DEBUG_H_
#define _AT91_DEBUG_H_

#include <types.h>
#include <embox/kernel.h>

/* Init leds */
extern void led_init();

/* Set first led on */
extern void led1_on();

/* Set first led off */
extern void led1_off();

/* Set second led on */
extern void led2_on();

/* Set second led off */
extern void led2_off();

#endif /* _AT91_DEBUG_H_ */
