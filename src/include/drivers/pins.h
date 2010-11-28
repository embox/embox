/**
 * @file
 * @brief Pins system interface
 *
 * @date 26.11.2010
 * @author Anton Kozlov
 */

#ifndef PINS_H_
#define PINS_H_

#include <hal/pins.h>

typedef void (*pin_handler_t)(int mask);

extern void pin_set_input_monitor(int mask, pin_handler_t pin_handled);

#endif /* PINS_H_ */
