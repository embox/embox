/**
 * @file
 * @brief Pins interface
 *
 * @date 26.11.10
 * @author Anton Kozlov
 */

#ifndef HAL_PINS_H_
#define HAL_PINS_H_

#include <drivers/pins.h>
#include <kernel/irq.h>

/**
 * Configure pin mask for input
 */
extern void pin_config_input(pin_mask_t mask);

/**
 * Configure pin mask for ouput
 */
extern void pin_config_output(pin_mask_t mask);

/**
 * Get data of input pins
 */
extern pin_mask_t pin_get_input(pin_mask_t mask);

/**
 * Set data for output pins
 */
extern void pin_set_output(pin_mask_t mask);

/**
 * Enable interrupt on change of pins
 */
extern void pin_set_input_interrupt(pin_mask_t mask);

/**
 * Get pins changed since last call
 */
extern int pin_get_input_changed(void);

extern irq_return_t irq_pin_handler(irq_nr_t irq_nr, void *data);

#endif /* HAL_PINS_H_ */
