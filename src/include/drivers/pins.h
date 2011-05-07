/**
 * @file
 * @brief Pins system interface
 *
 * @date 26.11.10
 * @author Anton Kozlov
 */

#ifndef PINS_H_
#define PINS_H_

typedef int pin_mask_t;

#include <hal/pins.h>

/**
 * @param ch_mask Monitored pins state
 * @param mon_mask Monitored pins
 */
typedef void (*pin_handler_t)(pin_mask_t ch_mask, pin_mask_t mon_mask);

/**
 * Monitor changing of mask bits, on change pin_handler will be called
 */
extern void pin_set_input_monitor(pin_mask_t mask, pin_handler_t pin_handled);

/**
 * Configure pins to output
 */
extern void pin_config_output(pin_mask_t mask);

/**
 * Set output pins to HIGH
 */
extern void pin_set_output(pin_mask_t mask);

/**
 * Set output pins to LOW
 */
extern void pin_clear_output(pin_mask_t mask);

#endif /* PINS_H_ */
