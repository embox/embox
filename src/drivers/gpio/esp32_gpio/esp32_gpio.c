#include <errno.h>
#include <stdint.h>

#include <drivers/gpio.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <kernel/irq.h>

#include <config/board_config.h>

#include "esp32_gpio.h"

static volatile struct esp32_gpio_regs *const gpio =
    (volatile struct esp32_gpio_regs *)CONF_GPIO_PORT_A_REGION_BASE;

static volatile struct esp32_io_mux_regs *const io_mux =
    (volatile struct esp32_io_mux_regs *)CONF_GPIO_PORT_A_REGION_IO_MUX;

static int esp32_gpio_setup_mode(unsigned int port, gpio_mask_t pins,
    uint32_t mode) {
	uint32_t pin;
	uint32_t func = ESP32_IO_MUX_FUNC_GPIO;
	uint32_t supported = GPIO_MODE_IN | GPIO_MODE_OUT
	    | GPIO_MODE_IN_PULL_UP | GPIO_MODE_IN_PULL_DOWN
	    | GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_OUT_OPEN_DRAIN
	    | GPIO_MODE_ALT_SECTION;

	if (port != GPIO_PORT_A
	    || (pins & ~ESP32_GPIO_PIN_MASK)) {
		return -EINVAL;
	}
	if ((mode & (GPIO_MODE_IN_PULL_UP | GPIO_MODE_IN_PULL_DOWN))
	        == (GPIO_MODE_IN_PULL_UP | GPIO_MODE_IN_PULL_DOWN)
	    || (mode & (GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_OUT_OPEN_DRAIN))
	        == (GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_OUT_OPEN_DRAIN)) {
		return -EINVAL;
	}
	if (!(mode & GPIO_MODE_OUT)
	    && (mode & (GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_OUT_OPEN_DRAIN))) {
		return -EINVAL;
	}
	if (mode & GPIO_MODE_ALT) {
		func = GPIO_MODE_ALT_GET(mode);
		if (func > 7U) {
			return -EINVAL;
		}
	} else if (mode & GPIO_MODE_ALT_SECTION) {
		return -EINVAL;
	}
	/* Interrupts, Schmitt control and voltage selection are not implemented. */
	if (mode & ~supported) {
		return -ENOTSUP;
	}

	/* Clear software output enable before changing the pad configuration. */
	gpio->enable_w1tc = pins;
	for (pin = 0; pin < ESP32_GPIO_PIN_COUNT; pin++) {
		uint32_t mux;
		uint32_t cfg;

		if (!(pins & (1U << pin))) {
			continue;
		}
		mux = io_mux->pin[pin];
		mux &= ~(ESP32_IO_MUX_FUNC_MASK | ESP32_IO_MUX_INPUT_ENABLE
		    | ESP32_IO_MUX_PULL_UP | ESP32_IO_MUX_PULL_DOWN);
		mux |= func << ESP32_IO_MUX_FUNC_SHIFT;
		if (mode & GPIO_MODE_IN) {
			mux |= ESP32_IO_MUX_INPUT_ENABLE;
		}
		if (mode & GPIO_MODE_IN_PULL_UP) {
			mux |= ESP32_IO_MUX_PULL_UP;
		}
		if (mode & GPIO_MODE_IN_PULL_DOWN) {
			mux |= ESP32_IO_MUX_PULL_DOWN;
		}
		cfg = gpio->pin[pin] & ~ESP32_GPIO_PAD_DRIVER;
		if (mode & GPIO_MODE_OUT_OPEN_DRAIN) {
			cfg |= ESP32_GPIO_PAD_DRIVER;
		}
		gpio->pin[pin] = cfg;
		if (!(mode & GPIO_MODE_ALT)) {
			/* Simple GPIO uses GPIO_OUT and GPIO_ENABLE, not peripheral OE. */
			gpio->func_out_sel_cfg[pin] = ESP32_GPIO_OUT_GPIO
			    | ESP32_GPIO_OUT_OE_SEL;
		}
		io_mux->pin[pin] = mux;
	}
	if (mode & GPIO_MODE_OUT) {
		gpio->enable_w1ts = pins;
	}

	return 0;
}

static int esp32_gpio_setup_route(unsigned int port, uint32_t pin,
    uint32_t signal) {
	uint32_t index = signal & GPIO_ROUTE_SIGNAL_MASK;

	if (port != GPIO_PORT_A || pin >= ESP32_GPIO_PIN_COUNT
	    || index >= ESP32_GPIO_SIGNAL_COUNT) {
		return -EINVAL;
	}

	if (signal & GPIO_ROUTE_INPUT) {
		gpio->func_in_sel_cfg[index] = pin | ESP32_GPIO_SIG_IN_SEL;
	} else {
		/* No inversion; output enable comes from the peripheral. */
		gpio->func_out_sel_cfg[pin] = index;
	}

	return 0;
}

static void esp32_gpio_set(unsigned int port, gpio_mask_t pins, int level) {
	if (port != GPIO_PORT_A) {
		return;
	}

	pins &= ESP32_GPIO_PIN_MASK;
	if (level) {
		gpio->out_w1ts = pins;
	} else {
		gpio->out_w1tc = pins;
	}
}

static gpio_mask_t esp32_gpio_get(unsigned int port, gpio_mask_t pins) {
	if (port != GPIO_PORT_A) {
		return 0;
	}

	return gpio->in & pins & ESP32_GPIO_PIN_MASK;
}
static const struct gpio_chip esp32_gpio_chip = {
    .setup_mode = esp32_gpio_setup_mode,
    .setup_route = esp32_gpio_setup_route,
    .get = esp32_gpio_get,
    .set = esp32_gpio_set,
#if defined(CONF_GPIO_PORT_A_NUM)
    .nports = CONF_GPIO_PORT_A_NUM,
#else
    .nports = 8,
#endif
    .chip_id = 0,
};

GPIO_CHIP_DEF(&esp32_gpio_chip);
