/**
 * @file
 * @brief OMAP3 GPIO pins subsystem implementation
 *
 * @date 1.11.13
 * @author Alexander Kalmuk
 */

#include <embox/unit.h>
#include <hal/reg.h>
#include <drivers/pins.h>
#include <hal/pins.h>
#include "omap3_gpio.h"

#define GPIO6_MODULE 6 // FIXME

void pin_config_input(pin_mask_t mask) {
	uint32_t l = gpio_reg_read(GPIO6_MODULE, GPIO_OE);
	gpio_reg_write(GPIO6_MODULE, GPIO_OE, l | mask);
}

void pin_config_output(pin_mask_t mask) {
	uint32_t l = gpio_reg_read(GPIO6_MODULE, GPIO_OE);
	gpio_reg_write(GPIO6_MODULE, GPIO_OE, l & ~mask);
}

pin_mask_t pin_get_input(pin_mask_t mask) {
	return mask & ((int) gpio_reg_read(GPIO6_MODULE, GPIO_DATAIN));
}

void pin_set_output(pin_mask_t mask) {
	uint32_t l = gpio_reg_read(GPIO6_MODULE, GPIO_DATAOUT);
	gpio_reg_write(GPIO6_MODULE, GPIO_DATAOUT, l | mask);
}

void pin_clear_output(pin_mask_t mask) {
	uint32_t l = gpio_reg_read(GPIO6_MODULE, GPIO_CLEARDATAOUT);
	gpio_reg_write(GPIO6_MODULE, GPIO_CLEARDATAOUT, l | mask);
}

void pin_set_input_interrupt(pin_mask_t mask) {
	uint32_t l = gpio_reg_read(GPIO6_MODULE, GPIO_IRQENABLE1);
	gpio_reg_write(GPIO6_MODULE, GPIO_IRQENABLE1, l | mask);
}

pin_mask_t pin_get_input_changed(void) {
	return (int) gpio_reg_read(GPIO6_MODULE, GPIO_IRQSTATUS1);
}
