/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    17.07.2024
 */

#include <stdint.h>

#include <drivers/gpio.h>
#include <hal/reg.h>
#include <kernel/irq.h>

//#include <config/board_config.h>
#include <drivers/clk/mikron_pm.h>
#include <framework/mod/options.h>

#define GPIO_CHIP_ID     OPTION_GET(NUMBER, gpio_chip_id)
#define GPIO_PINS_NUMBER 16

#define CONF_GPIO_PORT_0_REGION_BASE 0x00084000
#define CONF_GPIO_PORT_1_REGION_BASE 0x00084400
#define CONF_GPIO_PORT_2_REGION_BASE 0x00084800

#define CONF_GPIO_PORT_0_CLK_ENABLE() "CLK_GPIO_PORT0"
#define CONF_GPIO_PORT_1_CLK_ENABLE() "CLK_GPIO_PORT1"
#define CONF_GPIO_PORT_2_CLK_ENABLE() "CLK_GPIO_PORT2"

#define GPIOA ((volatile struct gpio_reg *)CONF_GPIO_PORT_0_REGION_BASE)
#define GPIOB ((volatile struct gpio_reg *)CONF_GPIO_PORT_1_REGION_BASE)
#define GPIOC ((volatile struct gpio_reg *)CONF_GPIO_PORT_2_REGION_BASE)

extern void mik_pad_cfg_set_func(int port, int pin, int func);
extern void mik_pad_cfg_set_ds(int port, int pin, int ds);
extern void mik_pad_cfg_set_pupd(int port, int pin, int pupd);

struct gpio_reg {
	union {
		volatile uint32_t SET;
		volatile uint32_t STATE;
	};
	volatile uint32_t CLEAR;
	volatile uint32_t DIRECTION_OUT;
	volatile uint32_t DIRECTION_IN;
	volatile uint32_t OUTPUT;
	volatile uint32_t CONTROL;
};

static const struct gpio_chip mik_gpio_chip;

static inline void mik_gpio_irq_en(volatile struct gpio_reg *gpio_reg,
    uint32_t mask) {
}

static inline void mik_gpio_irq_dis(volatile struct gpio_reg *gpio_reg,
    uint32_t mask) {
}

static inline uint32_t mik_gpio_irq_get_status(
    volatile struct gpio_reg *gpio_reg) {
	return 0;
}

static inline void mik_gpio_irq_clear_status(volatile struct gpio_reg *gpio_reg,
    uint32_t mask) {
}

static inline volatile struct gpio_reg *mik_gpio_get_gpio_port(
    unsigned int port) {
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

irq_return_t mik_gpio_irq_handler(unsigned int irq_nr, void *gpio) {
	uint32_t mask = 0;
	uint8_t port_num = 0;
	volatile struct gpio_reg *gpio_reg = gpio;

	mask = mik_gpio_irq_get_status(gpio_reg);

	mik_gpio_irq_clear_status(gpio_reg, mask);

	gpio_handle_irq(&mik_gpio_chip, port_num, mask);

	return IRQ_HANDLED;
}

static int mik_gpio_setup_irq(int port, uint32_t mask, uint32_t mode) {
	int res;
	volatile struct gpio_reg *gpio_reg;
	// int type;
	// int edge;
	// int pol;

	gpio_reg = mik_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return -1;
	}

	res = irq_attach(0, mik_gpio_irq_handler, 0, (void *)gpio_reg, "GPIO Irq");
	if (res < 0) {
		return res;
	}

	if ((mode & GPIO_MODE_INT_MODE_LEVEL0)
	    || (mode & GPIO_MODE_INT_MODE_LEVEL1)) {}
	else {}

	if ((mode & GPIO_MODE_INT_MODE_RISING)
	    && (mode & GPIO_MODE_INT_MODE_FALLING)) {}
	else {}

	if ((mode & GPIO_MODE_INT_MODE_RISING)
	    || (mode & GPIO_MODE_INT_MODE_LEVEL1)) {}
	else {}

	if (mode & GPIO_MODE_IN_INT_EN) {
		mik_gpio_irq_en(gpio_reg, mask);
	}

	return res;
}

static int mik_gpio_setup_mode(unsigned int port, gpio_mask_t pins,
    uint32_t mode) {
	volatile struct gpio_reg *gpio_reg;
	char *clk_name = NULL;
	uint32_t alt = 0;
	uint32_t pull = 0;

	gpio_reg = mik_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return -1;
	}

	switch (port) {
	case 0:
		clk_name = CONF_GPIO_PORT_0_CLK_ENABLE();
		break;
	case 1:
		clk_name = CONF_GPIO_PORT_1_CLK_ENABLE();
		break;
	case 2:
		clk_name = CONF_GPIO_PORT_2_CLK_ENABLE();
		break;
	default:
		return -1;
	}

	/* Enable port */
	clk_enable(clk_name);

	for (int pin = 0; pin < GPIO_PINS_NUMBER; pin++) {
		if (!(pins & (1 << pin))) {
			continue;
		}

		if (mode & GPIO_MODE_IN) {
			gpio_reg->DIRECTION_IN |= 1 << pin;
			alt = 0;
		}

		if (mode & GPIO_MODE_OUT) {
			gpio_reg->DIRECTION_OUT |= 1 << pin;
			alt = 0;
		}

		if (mode & GPIO_MODE_IN_PULL_UP) {
			pull = 1;
		}

		if (mode & GPIO_MODE_IN_PULL_DOWN) {
			pull = 2;
		}

		if (mode & GPIO_MODE_ALT_SECTION) {
			alt = GPIO_MODE_ALT_GET(mode);
		}

		mik_pad_cfg_set_func(port, pin, alt);
		mik_pad_cfg_set_ds(port, pin, 0);
		mik_pad_cfg_set_pupd(port, pin, pull);
	}

	if (mode & GPIO_MODE_INT_SECTION) {
		mik_gpio_setup_irq(port, pins, mode);
	}

	return 0;
}

static void mik_gpio_set(unsigned int port, gpio_mask_t pins, int level) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = mik_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return;
	}

	if (level) {
		gpio_reg->SET |= pins;
	}
	else {
		gpio_reg->CLEAR |= pins;
	}
}

static gpio_mask_t mik_gpio_get(unsigned int port, gpio_mask_t pins) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = mik_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return -1;
	}

	return gpio_reg->SET & pins;
}

static const struct gpio_chip mik_gpio_chip = {
    .setup_mode = mik_gpio_setup_mode,
    .get = mik_gpio_get,
    .set = mik_gpio_set,
    .nports = 3,
    .chip_id = GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&mik_gpio_chip);

//STATIC_IRQ_ATTACH(GPIO_IRQ, gpio_irq_handler, NULL);
