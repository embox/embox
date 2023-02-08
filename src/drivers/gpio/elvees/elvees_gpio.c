/**
 * @file
 *
 * @date Jan 24, 2023
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stdint.h>
#include <stddef.h>
#include <sys/mman.h>

#include <hal/reg.h>

#include <drivers/common/memory.h>
#include <drivers/gpio/gpio.h>
#include <drivers/gpio/gpio_driver.h>

#include <framework/mod/options.h>


#define IOCTR_BASE_ADDR(port_num) \
	((uintptr_t) OPTION_GET(NUMBER,base_addr) + (port_num) * 0x100)

#define GPIO_PORTS_COUNT           OPTION_GET(NUMBER,gpio_ports)


#define IOCTR_MODE           0x00
#define IOCTR_AFL            0x04
#define IOCTR_AHL            0x08


#define IOCTR_MODE_ADDR(port_num)     (IOCTR_BASE_ADDR(port) + IOCTR_MODE)
#define IOCTR_AFL_ADDR(port_num)      (IOCTR_BASE_ADDR(port) + IOCTR_AFL)
#define IOCTR_AHL_ADDR(port_num)      (IOCTR_BASE_ADDR(port) + IOCTR_AHL)

#define GPIO_MODE_AF         (2)

static void elvees_gpio_setup_func(uint32_t port, uint32_t pin, int func) {
	uint32_t *addr_reg;
	uint32_t reg = GPIO_MODE_AF << (pin * 2);

	addr_reg = (uint32_t *)IOCTR_MODE_ADDR(port);

	reg = REG32_LOAD(addr_reg);
	reg &= ~(3 << (pin * 2));
	reg |= GPIO_MODE_AF << (pin * 2);
	REG32_STORE(addr_reg, reg);

	if (pin < 8) {
		addr_reg = (uint32_t *)IOCTR_AFL_ADDR(port);
	} else {
		pin %= 8;
		addr_reg = (uint32_t *)IOCTR_AHL_ADDR(port);
	}

	reg = REG32_LOAD(addr_reg);
	reg &= ~(7 << (pin * 4));
	reg |= func << (pin * 4);
	REG32_STORE(addr_reg, reg);
}

static int elvees_gpio_setup_mode(unsigned char port, gpio_mask_t mask, int m) {
	int i;

	for (i = 0; i < 16; i++) {
		if (mask & (1 << i)) {
			if (m & GPIO_MODE_OUT_ALTERNATE) {
				elvees_gpio_setup_func(port, i, GPIO_GET_ALTERNATE(m));
			}
		}
	}

	return 0;
}

static void elvees_gpio_set(unsigned char port, gpio_mask_t mask, char level) {
}

static gpio_mask_t elvees_gpio_get(unsigned char port, gpio_mask_t mask) {
	return 0;
}

struct gpio_chip elvees_gpio_chip = {
	.setup_mode = elvees_gpio_setup_mode,
	.get = elvees_gpio_get,
	.set = elvees_gpio_set,
	.nports = GPIO_PORTS_COUNT
};

GPIO_CHIP_DEF(&elvees_gpio_chip);

PERIPH_MEMORY_DEFINE(elvees_gpio, IOCTR_BASE_ADDR(0), 0x100 * GPIO_PORTS_COUNT);
