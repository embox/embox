/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    17.07.2024
 */

#include <stdint.h>

#include <drivers/gpio/gpio_driver.h>
#include <hal/reg.h>
#include <kernel/irq.h>

//#include <config/board_config.h>
#include <drivers/clk/mikron_pm.h>

#include <framework/mod/options.h>

#define GPIO_CHIP_ID                 OPTION_GET(NUMBER, gpio_chip_id)
#define GPIO_PINS_NUMBER             16

#define CONF_GPIO_PORT_0_REGION_BASE 0x00084000
#define CONF_GPIO_PORT_1_REGION_BASE 0x00084400
#define CONF_GPIO_PORT_2_REGION_BASE 0x00084800

#define GPIOA                        ((volatile struct gpio_reg *)CONF_GPIO_PORT_0_REGION_BASE)
#define GPIOB                        ((volatile struct gpio_reg *)CONF_GPIO_PORT_1_REGION_BASE)
#define GPIOC                        ((volatile struct gpio_reg *)CONF_GPIO_PORT_2_REGION_BASE)

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

static struct gpio_chip mik_gpio_chip;

static inline void mik_gpio_irq_conf_type(volatile struct gpio_reg *gpio_reg,
    uint32_t mask, int type) {
}

static inline void mik_gpio_irq_conf_pol(volatile struct gpio_reg *gpio_reg,
    uint32_t mask, int pol) {
}

static inline
void mik_gpio_irq_conf_edge(volatile struct gpio_reg *gpio_reg,
			uint32_t mask, int edge) {
}

static inline
void mik_gpio_irq_en(volatile struct gpio_reg *gpio_reg,
			uint32_t mask) {

}

static inline void mik_gpio_irq_dis(volatile struct gpio_reg *gpio_reg,
				uint32_t mask) {
}

static inline uint32_t mik_gpio_irq_get_status(
					volatile struct gpio_reg *gpio_reg) {
	return 0;

}

static inline void mik_gpio_irq_clear_status(
		volatile struct gpio_reg *gpio_reg, uint32_t mask) {

}

static inline volatile struct gpio_reg *mik_gpio_get_gpio_port(
	unsigned char port) {
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

static int mik_gpio_setup_irq(int port, uint32_t mask, int mode) {
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
	    || (mode & GPIO_MODE_INT_MODE_LEVEL1)) {
		//type = 0;
	}
	else {
		//type = 1;
	}

	if ((mode & GPIO_MODE_INT_MODE_RISING)
	    && (mode & GPIO_MODE_INT_MODE_FALLING)) {
		//edge = 1;
	}
	else {
		//edge = 0;
	}

	if ((mode & GPIO_MODE_INT_MODE_RISING)
	    || (mode & GPIO_MODE_INT_MODE_LEVEL1)) {
		//pol = 1;
	}
	else {
		//pol = 0;
	}

	// mik_gpio_irq_conf_pol(gpio_reg, mask, pol);
	// mik_gpio_irq_conf_edge(gpio_reg, mask, edge);
	// mik_gpio_irq_conf_type(gpio_reg, mask, type);

	// if (mode & GPIO_MODE_IN_INT_EN) {
	// 	mik_gpio_irq_en(gpio_reg, mask);
	// }


	return res;
}

static int mik_gpio_setup_mode(unsigned char port, gpio_mask_t pins,
    int mode) {
	volatile struct gpio_reg *gpio_reg;
	char *clk_name = NULL;

	gpio_reg = mik_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return -1;
	}

	switch (port) {
	case 0:
		//clk_name = CONF_GPIO_PORT_A_CLK_ENABLE();
		break;
	case 1:
		//clk_name = CONF_GPIO_PORT_B_CLK_ENABLE();
		break;
	case 2:
		//clk_name = CONF_GPIO_PORT_C_CLK_ENABLE();
		break;
	default:
		return -1;
	}

	/* Enable port */
	clk_enable(clk_name);

	if (mode & GPIO_MODE_INT_SECTION) {
		mik_gpio_setup_irq(port, pins, mode);
	}


	return 0;
}

static void mik_gpio_set(unsigned char port, gpio_mask_t pins, char level) {
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

static gpio_mask_t mik_gpio_get(unsigned char port, gpio_mask_t pins) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = mik_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return -1;
	}

	return gpio_reg->SET & pins;
}

static struct gpio_chip mik_gpio_chip = {
	.setup_mode = mik_gpio_setup_mode,
    .get = mik_gpio_get,
    .set = mik_gpio_set,
    .nports = 3
};

GPIO_CHIP_DEF(&mik_gpio_chip);

//STATIC_IRQ_ATTACH(GPIO_IRQ, gpio_irq_handler, NULL);
