/**
 * @file
 *
 * @date 20.01.2017
 * @author Anton Bondarev
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <sys/mman.h>

#include <util/log.h>

#include <hal/reg.h>
#include <drivers/common/memory.h>

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
	struct gpio_dwapb_port *gpio_port;
	uint32_t direction;
	uint32_t ctl;

	gpio_port = (void *)(BASE_CTRL_ADDR + gpio->port_id * sizeof(struct gpio_dwapb_port));

	log_debug("%p mask 0x%X mode %d", gpio_port, mask, mode);
	ctl = REG32_LOAD(&gpio_port->ctl);
	ctl &= ~mask;
	REG32_STORE(&gpio_port->ctl, ctl); /* all hardware pins */
	switch (mode) {
	case GPIO_MODE_OUTPUT:
		break;
	default:
		log_error("wrong gpio mode");
		return -EINVAL;
	}

	direction = REG32_LOAD(&gpio_port->ddr);
	REG32_STORE(&gpio_port->ddr, direction | mask);

	return 0;
}

void gpio_set_level(struct gpio *gpio, gpio_mask_t mask, char level) {
	struct gpio_dwapb_port *gpio_port;
	uint32_t dr;

	gpio_port = (void *)(BASE_CTRL_ADDR + gpio->port_id * sizeof(struct gpio_dwapb_port));
	dr = REG32_LOAD(&gpio_port->dr);
	if (level) {
		dr |= mask;

	} else {
		dr &= ~mask;
	}
	log_debug("%p mask 0x%X mode %d", gpio_port, mask, level);
	REG32_STORE(&gpio_port->dr, dr);
}

gpio_mask_t gpio_get_level(struct gpio *gpio, gpio_mask_t mask) {
	return 0;
}

static struct periph_memory_desc arasan_mem = {
	.start = BASE_CTRL_ADDR,
	.len   = 0x200
};

PERIPH_MEMORY_DEFINE(arasan_mem);

