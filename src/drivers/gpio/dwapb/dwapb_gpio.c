/**
 * @file
 *
 * @date 20.01.2017
 * @author Anton Bondarev
 */
#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <drivers/gpio.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <util/log.h>

#define GPIO_CHIP_ID OPTION_GET(NUMBER, gpio_chip_id)
#define BASE_CTRL_ADDR(i) \
	((uintptr_t)OPTION_GET(NUMBER, base_addr) + (i)*0x1000)

#define DWAPB_GPIO_PORTS_COUNT OPTION_GET(NUMBER, gpio_ports)

EMBOX_UNIT_INIT(dwapb_gpio_init);

struct gpio_dwapb_port {
	uint32_t dr;  /* data */
	uint32_t ddr; /* direction */
	uint32_t ctl;
};

static int dwapb_gpio_setup_mode(unsigned int port, gpio_mask_t mask,
    uint32_t mode) {
	struct gpio_dwapb_port *gpio_port;
	uint32_t direction;
	uint32_t ctl;

	gpio_port = (struct gpio_dwapb_port *)BASE_CTRL_ADDR(port);

	log_debug("port %d mask 0x%X mode %d", port, mask, mode);
	ctl = REG32_LOAD(&gpio_port->ctl);
	ctl &= ~mask;
	REG32_STORE(&gpio_port->ctl, ctl); /* all hardware pins */
	switch (mode) {
	case GPIO_MODE_OUT:
		break;
	default:
		log_error("wrong gpio mode");
		return -EINVAL;
	}

	direction = REG32_LOAD(&gpio_port->ddr);
	REG32_STORE(&gpio_port->ddr, direction | mask);

	return 0;
}

static void dwapb_gpio_set(unsigned int port, gpio_mask_t mask, int level) {
	struct gpio_dwapb_port *gpio_port;
	uint32_t dr;

	gpio_port = (struct gpio_dwapb_port *)BASE_CTRL_ADDR(port);
	dr = REG32_LOAD(&gpio_port->dr);
	if (level) {
		dr |= mask;
	}
	else {
		dr &= ~mask;
	}
	log_debug("%d mask 0x%X mode %d", port, mask, level);
	REG32_STORE(&gpio_port->dr, dr);
}

static gpio_mask_t dwapb_gpio_get(unsigned int port, gpio_mask_t mask) {
	struct gpio_dwapb_port *gpio_port;
	uint32_t dr;

	gpio_port = (struct gpio_dwapb_port *)BASE_CTRL_ADDR(port);
	dr = REG32_LOAD(&gpio_port->dr);

	return dr & mask;
}

static const struct gpio_chip dwapb_gpio_chip = {
    .setup_mode = dwapb_gpio_setup_mode,
    .get = dwapb_gpio_get,
    .set = dwapb_gpio_set,
    .nports = DWAPB_GPIO_PORTS_COUNT,
    .chip_id = GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&dwapb_gpio_chip);

static int dwapb_gpio_init(void) {
	return 0;
}

PERIPH_MEMORY_DEFINE(arasan, BASE_CTRL_ADDR(0),
    0x1000 * DWAPB_GPIO_PORTS_COUNT);
