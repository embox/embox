/**
 * @file
 * @brief Pins interface
 *
 * @date 26.11.2010
 * @author Anton Kozlov
 */

#ifndef HAL_PINS_H_
#define HAL_PINS_H_

/**
 * Configure pin mask for input
 */
extern void pin_config_input(int mask);

/**
 * Configure pin mask for ouput
 */
extern void pin_config_output(int mask);

/**
 * Get data of input pins
 */
extern int pin_get_input(void);

/**
 * Set data for output pins
 */
extern void pin_set_output(int mask);

/**
 * Enable interrupt on change of pins
 */
extern void pin_set_input_interrupt(int mask);

/**
 * Get pins changed since last call
 */
extern int pin_get_input_changed(void);

extern irq_return_t irq_pin_handler(irq_nr_t irq_nr, void *data);
#endif /* HAL_PINS_H_ */
