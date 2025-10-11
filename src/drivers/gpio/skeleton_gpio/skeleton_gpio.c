/**
 * @file
 * @brief
 *
 * @author  Andrew Bursian
 * @date    21.02.2023
 */

#include <stdint.h>

#include <drivers/clk/bmcu_cru_drv.h>
#include <drivers/gpio.h>
#include <hal/reg.h>
#include <kernel/irq.h>

#include <framework/mod/options.h>
#include <config/board_config.h>

#define GPIO_CHIP_ID     OPTION_GET(NUMBER, gpio_chip_id)
#define GPIO_PINS_NUMBER 16

#define GPIOA ((volatile struct gpio_regs *)CONF_GPIO_PORT_A_REGION_BASE)
#define GPIOB ((volatile struct gpio_regs *)CONF_GPIO_PORT_B_REGION_BASE)
#define GPIOC ((volatile struct gpio_regs *)CONF_GPIO_PORT_C_REGION_BASE)


struct gpio_regs {
    volatile uint32_t DR;               /*!< Data                                   Address offset: 0x00 */
    volatile uint32_t DDR;              /*!< Direction                              Address offset: 0x04 */
};

static const struct gpio_chip skeleton_gpio_chip;

static inline volatile struct gpio_regs *skeleton_gpio_get_gpio_port(int port) {
	switch (port) {
	case 0:
		return GPIOA;
	case 1:
		return GPIOB;
	case 2:
		return GPIOC;
		break;
	default:
		return NULL;
	}

	return 0;
}

static int skeleton_gpio_setup_mode(unsigned int port, gpio_mask_t pins, uint32_t mode) {
	volatile struct gpio_regs *gpio_regs;
	char *clk_name;

	gpio_regs = skeleton_gpio_get_gpio_port(port);
	if (gpio_regs == NULL) {
		return -1;
	}

	switch (port) {
	case 0:
		clk_name = CONF_GPIO_PORT_A_CLK_ENABLE();
		break;
	case 1:
		clk_name = CONF_GPIO_PORT_B_CLK_ENABLE();
		break;
	case 2:
		clk_name = CONF_GPIO_PORT_C_CLK_ENABLE();
		break;
	default:
		return -1;
	}

	/* Enable port */
	clk_enable(clk_name);

	if (mode & GPIO_MODE_IN) {
		gpio_regs->DDR |= pins;
	}

	if (mode & GPIO_MODE_OUT) {
		gpio_regs->DDR |= pins;
	}

	if (mode & GPIO_MODE_INT_SECTION) {
		// skeleton_gpio_setup_irq(port, pins, mode);
	}

	if (mode & GPIO_MODE_ALT_SECTION) {
		for (int i = 0; i < GPIO_PINS_NUMBER; i++) {
			if (pins & (1 << i)) {
				/* Enable ALTFUNC */
				uint32_t alt = GPIO_MODE_ALT_GET(mode);
				bmcu_cru_pin_altfunc_set(port, i, alt);

			}
		}
	}

	return 0;
}

static void skeleton_gpio_set(unsigned int port, gpio_mask_t pins, int level) {
	volatile struct gpio_regs *gpio_regs;

	gpio_regs = skeleton_gpio_get_gpio_port(port);
	if (gpio_regs == NULL) {
		return;
	}

	if (level) {
		gpio_regs->DR |= pins;
	}
	else {
		gpio_regs->DR &= ~pins;
	}
}

static gpio_mask_t skeleton_gpio_get(unsigned int port, gpio_mask_t pins) {
	volatile struct gpio_regs *gpio_regs;

	gpio_regs = skeleton_gpio_get_gpio_port(port);
	if (gpio_regs == NULL) {
		return -1;
	}

	return gpio_regs->DR & pins;
}

static const struct gpio_chip skeleton_gpio_chip = {
    .setup_mode = skeleton_gpio_setup_mode,
    .get = skeleton_gpio_get,
    .set = skeleton_gpio_set,
    .nports = 3,
    .chip_id = GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&skeleton_gpio_chip);

