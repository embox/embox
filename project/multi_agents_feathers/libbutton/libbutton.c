/**
 * @file
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-04-04
 */

#include <drivers/gpio/gpio_driver.h>

#include <feather/libbutton.h>

#define BUTTON_PORT  0 /* PORTA0 */
#define BUTTON_PIN   0 /* PORTA0 */

void libbutton_init(void) {
	gpio_setup_mode(BUTTON_PORT, (1 << BUTTON_PIN), GPIO_MODE_IN);
}

int libbutton_get_state(void) {
	return (1 << BUTTON_PIN) & gpio_get(BUTTON_PORT, (1 << BUTTON_PIN));
}