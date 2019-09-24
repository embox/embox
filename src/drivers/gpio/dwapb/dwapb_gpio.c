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
#include <embox/unit.h>

#include <hal/reg.h>
#include <drivers/common/memory.h>

#include <drivers/gpio/gpio_driver.h>

#define GPIO_CHIP_ID OPTION_GET(NUMBER,gpio_chip_id)
#define BASE_CTRL_ADDR ((uintptr_t) OPTION_GET(NUMBER,base_addr))

#define DWAPB_GPIO_PORTS_COUNT 4

EMBOX_UNIT_INIT(dwapb_gpio_init);

struct gpio_dwapb_port {
	uint32_t dr; /* data */
	uint32_t ddr; /* direction */
	uint32_t ctl;
};

static int dwapb_gpio_setup_mode(unsigned char port, gpio_mask_t mask, int mode) {
	struct gpio_dwapb_port *gpio_port;
	uint32_t direction;
	uint32_t ctl;

	gpio_port = (void *)(BASE_CTRL_ADDR + port * sizeof(struct gpio_dwapb_port));

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

static void dwapb_gpio_set(unsigned char port, gpio_mask_t mask, char level) {
	struct gpio_dwapb_port *gpio_port;
	uint32_t dr;

	gpio_port = (void *)(BASE_CTRL_ADDR + port * sizeof(struct gpio_dwapb_port));
	dr = REG32_LOAD(&gpio_port->dr);
	if (level) {
		dr |= mask;

	} else {
		dr &= ~mask;
	}
	log_debug("%p mask 0x%X mode %d", gpio_port, mask, level);
	REG32_STORE(&gpio_port->dr, dr);
}

static gpio_mask_t dwapb_gpio_get(unsigned char port, gpio_mask_t mask) {
	struct gpio_dwapb_port *gpio_port;
	uint32_t dr;

	gpio_port = (void *)(BASE_CTRL_ADDR + port * sizeof(struct gpio_dwapb_port));
	dr = REG32_LOAD(&gpio_port->dr);

	return dr & mask;
}

static struct gpio_chip dwapb_gpio_chip = {
	.setup_mode = dwapb_gpio_setup_mode,
	.get = dwapb_gpio_get,
	.set = dwapb_gpio_set,
	.nports = DWAPB_GPIO_PORTS_COUNT
};

static int dwapb_gpio_init(void) {
	return gpio_register_chip(&dwapb_gpio_chip, GPIO_CHIP_ID);
}

PERIPH_MEMORY_DEFINE(arasan, BASE_CTRL_ADDR, 0x200);
