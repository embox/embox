/**
 * @file
 * @brief Interface for controlling 2 leds on Olimex Debug board
 *
 * @date 26.09.2010
 * @author Anton Kozlov
 */

#ifndef AT91_OLIMEX_DEBUG_H_
#define AT91_OLIMEX_DEBUG_H_

#include <types.h>
#include <embox/kernel.h>

/* Init leds */
extern void led_init(void);

/* Set first led on */
extern void led1_on(void);

/* Set first led off */
extern void led1_off(void);

/* Set second led on */
extern void led2_on(void);

/* Set second led off */
extern void led2_off(void);

#endif /* AT91_OLIMEX_DEBUG_H_ */

