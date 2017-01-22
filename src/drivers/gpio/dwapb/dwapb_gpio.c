/**
 * @file
 *
 * @date 20.01.2017
 * @author Anton Bondarev
 */
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include <drivers/gpio.h>

#define BASE_CTRL_ADDR OPTION_GET(NUMBER,base_addr)

struct gpio_dwapb_port {
	uint32_t dr; /* data */
	uint32_t ddr; /* direction */
	uint32_t ctl;
};

static const struct gpio dwapb_port[4] = {{0}, {1}, {2}, {3}};

static int valid_port_num(int port_num) {
	if (port_num > 0 && port_num < 5) return 1;

	return 0;
}

struct gpio *gpio_by_num(int num_port) {
	assert(valid_port_num(num_port));

	return (struct gpio *)&dwapb_port[num_port - 1];
}

int gpio_settings(struct gpio *gpio, gpio_mask_t mask, int mode) {
	return 0;
}

void gpio_set_level(struct gpio *gpio, gpio_mask_t mask, char level) {

}

gpio_mask_t gpio_get_level(struct gpio *gpio, gpio_mask_t mask) {
	return 0;
}
