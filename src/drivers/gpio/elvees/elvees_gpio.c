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

#include <config/board_config.h>


#define GPIO_BASE_ADDR(port_num) \
	((uintptr_t) CONF_GPIO_PORT_A_REGION_BASE + (port_num) * 0x1000)

#define GPIO_PORTS_COUNT           OPTION_GET(NUMBER,gpio_ports)

#define DATA_IN        (0x00)
#define DATA_OUT       (0x04)
#define OUT_EN_SET     (0x10)
#define OUT_EN_CLR     (0x14)

#define GPIO_DATA_IN(port)    (GPIO_BASE_ADDR(port) + DATA_IN)
#define GPIO_DATA_OUT(port)    (GPIO_BASE_ADDR(port) + DATA_OUT)

#define GPIO_OUT_EN_SET(port)    (GPIO_BASE_ADDR(port) + OUT_EN_SET)
#define GPIO_OUT_EN_CLR(port)    (GPIO_BASE_ADDR(port) + OUT_EN_CLR)

extern void elvees_gpio_setup_func(uint32_t port, uint32_t pin, int func);
extern void elvees_gpio_setup_digit(uint32_t port, uint32_t pin);

static int elvees_gpio_setup_mode(unsigned char port, gpio_mask_t mask, int m) {
	int i;

	for (i = 0; i < 16; i++) {
		if (mask & (1 << i)) {
			if (m & GPIO_MODE_ALT_SECTION) {
				elvees_gpio_setup_func(port, i, GPIO_MODE_ALT_GET(m));
			}
			if (m & GPIO_MODE_OUT) {
				elvees_gpio_setup_digit(port, i);
				REG32_STORE(GPIO_OUT_EN_SET(port), (1 << i));
			}
			if (m & GPIO_MODE_IN) {
				elvees_gpio_setup_digit(port, i);
				REG32_STORE(GPIO_OUT_EN_CLR(port), (1 << i));
			}
		}
	}

	return 0;
}

static void elvees_gpio_set(unsigned char port, gpio_mask_t mask, char level) {

	if (level) {
		REG32_ORIN(GPIO_DATA_OUT(port), mask);
	} else {
		REG32_CLEAR(GPIO_DATA_OUT(port), mask);
	}
}

static gpio_mask_t elvees_gpio_get(unsigned char port, gpio_mask_t mask) {
	return REG32_LOAD(GPIO_DATA_IN(port)) & mask;
}

struct gpio_chip elvees_gpio_chip = {
	.setup_mode = elvees_gpio_setup_mode,
	.get = elvees_gpio_get,
	.set = elvees_gpio_set,
	.nports = GPIO_PORTS_COUNT
};

GPIO_CHIP_DEF(&elvees_gpio_chip);

PERIPH_MEMORY_DEFINE(elvees_gpio, GPIO_BASE_ADDR(0), 0x1000 * GPIO_PORTS_COUNT);
