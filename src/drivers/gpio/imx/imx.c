/**
 * @file imx.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 13.06.2017
 */

#include <drivers/common/memory.h>
#include <drivers/gpio/gpio_driver.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <util/log.h>

#define GPIO_CHIP_ID OPTION_GET(NUMBER,gpio_chip_id)
#define GPIO_PORTS   OPTION_GET(NUMBER, gpio_ports)
#define BASE_ADDR(n) ((OPTION_GET(NUMBER, base_addr)) + (n) * 0x4000)

#define GPIO_DR(n)         (BASE_ADDR(n) + 0x00)
#define GPIO_GDIR(n)       (BASE_ADDR(n) + 0x04)
#define GPIO_PSR(n)        (BASE_ADDR(n) + 0x08)
#define GPIO_ICR1(n)       (BASE_ADDR(n) + 0x0C)
#define GPIO_ICR2(n)       (BASE_ADDR(n) + 0x10)
#define GPIO_IMR(n)        (BASE_ADDR(n) + 0x14)
#define GPIO_ISR(n)        (BASE_ADDR(n) + 0x18)
#define GPIO_EDGE_SEL(n)   (BASE_ADDR(n) + 0x1C)

#define GPIO_DIR_IN     0
#define GPIO_DIR_OUT    1

EMBOX_UNIT_INIT(imx_gpio_init);

static struct gpio_chip imx_gpio_chip;

static int imx_gpio_init(void) {
	for (int i = 0; i <= GPIO_PORTS; i++) {
		log_debug("GPIO%d base address=%p", i, BASE_ADDR(i));
	}

	return 0;
}

static int imx_gpio_setup_mode(unsigned char port, gpio_mask_t mask, int mode) {
	uint32_t val = 0;
	uint32_t tmp;

	log_debug("Set GPIO%d;mask=0x%08x;mode=%d", port, mask, mode);

	switch (mode) {
	case GPIO_MODE_IN:
		val = mask * GPIO_DIR_IN;
		break;
	case GPIO_MODE_OUT:
		val = mask * GPIO_DIR_OUT;
		break;
	default:
		return -1;
	}

	tmp = REG32_LOAD(GPIO_GDIR(port));
	tmp &= ~mask;
	tmp |= val;
	REG32_STORE(GPIO_GDIR(port), tmp);

	return 0;
}

static void imx_gpio_set(unsigned char port, gpio_mask_t mask, char level) {
	log_debug("set level %d for GPIO#%d 0x%08x", level, port, mask);

	if (level == 0) {
		REG32_CLEAR(GPIO_DR(port), mask);
	} else {
		REG32_ORIN(GPIO_DR(port), mask);
	}
}

gpio_mask_t imx_gpio_get(unsigned char port, gpio_mask_t mask) {
	int ret = REG32_LOAD(GPIO_PSR(port)) & mask;

	log_debug("get level for GPIO#%d 0x%08x=0x%08x", port, mask, ret);

	return ret;
}

static struct gpio_chip imx_gpio_chip = {
	.setup_mode = imx_gpio_setup_mode,
	.get = imx_gpio_get,
	.set = imx_gpio_set,
	.nports = GPIO_PORTS,
	.chip_id = GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&imx_gpio_chip);

PERIPH_MEMORY_DEFINE(imx_gpio, BASE_ADDR(0), (0x4000 * 5));
