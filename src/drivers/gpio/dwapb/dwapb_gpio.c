/**
 * @file
 *
 * @date 20.01.2017
 * @author Anton Bondarev
 */
#include <stddef.h>
#include <drivers/gpio.h>


struct gpio *gpio_by_num(int num_port) {
	return NULL;
}

int gpio_settings(struct gpio *gpio, gpio_mask_t mask, int mode) {
	return 0;
}

void gpio_set_level(struct gpio *gpio, gpio_mask_t mask, char level) {

}

gpio_mask_t gpio_get_level(struct gpio *gpio, gpio_mask_t mask) {
	return 0;
}
