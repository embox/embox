/**
 * @file
 *
 * @date Feb 27, 2023
 * @author Anton Bondarev
 */

#include <util/log.h>

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

#define GPIO_MODE_DIG        (1)
#define GPIO_MODE_AF         (2)

void elvees_gpio_setup_func(uint32_t port, uint32_t pin, int func) {
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


void elvees_gpio_setup_digit(uint32_t port, uint32_t pin) {
	uint32_t *addr_reg;
	uint32_t reg = GPIO_MODE_AF << (pin * 2);

	addr_reg = (uint32_t *)IOCTR_MODE_ADDR(port);

	reg = REG32_LOAD(addr_reg);
	reg &= ~(3 << (pin * 2));
	reg |= GPIO_MODE_DIG << (pin * 2);
	REG32_STORE(addr_reg, reg);

}

PERIPH_MEMORY_DEFINE(elvees_gpio, IOCTR_BASE_ADDR(0), 0x100 * GPIO_PORTS_COUNT);
