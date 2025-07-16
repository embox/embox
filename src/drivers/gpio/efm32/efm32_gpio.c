/**
 * @file stm32_gpio_cube.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-12
 */

#include <assert.h>
#include <em_cmu.h>
#include <em_gpio.h>

#include <drivers/gpio.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <lib/libds/bit.h>

#include <module/platform/efm32/efm32_conf.h>

EMBOX_UNIT_INIT(efm32_gpio_init);

#define BSP_LED_POLARITY 1

#define EFM32_GPIO_CHIP_ID     OPTION_GET(NUMBER, gpio_chip_id)
#define EFM32_GPIO_PORTS_COUNT (OPTION_GET(NUMBER, gpio_ports_number) + 1)

static const const struct gpio_chip efm32_gpio_chip;

static int efm32_gpio_setup_mode(unsigned int port, gpio_mask_t pins,
    uint32_t mode) {
	int bit;

	switch (mode) {
	case GPIO_MODE_OUT: {
		bit_foreach(bit, pins) {
			if (pins & (1 << bit)) {
				GPIO_PinModeSet(port, bit, gpioModePushPull,
				    BSP_LED_POLARITY ? 0 : 1);
			}
		}
		break;
	}
	}

	return 0;
}

static void efm32_gpio_set(unsigned int port, gpio_mask_t pins, int level) {
	if (level) {
		GPIO_PortOutSet(port, pins);
	}
	else {
		GPIO_PortOutClear(port, pins);
	}
}

static gpio_mask_t efm32_gpio_get(unsigned int port, gpio_mask_t pins) {
	return GPIO_PortOutGet(port) & pins;
}

static const const struct gpio_chip efm32_gpio_chip = {
    .setup_mode = efm32_gpio_setup_mode,
    .get = efm32_gpio_get,
    .set = efm32_gpio_set,
    .nports = EFM32_GPIO_PORTS_COUNT,
    .chip_id = EFM32_GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&efm32_gpio_chip);

static int efm32_gpio_init(void) {
#if (_SILICON_LABS_32B_SERIES < 2)
	CMU_ClockEnable(cmuClock_HFPER, true);
#endif

#if (_SILICON_LABS_32B_SERIES < 2) \
    || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
	CMU_ClockEnable(cmuClock_GPIO, true);
#endif
	return 0;
}
