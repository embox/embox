/**
 * @file
 * 
 * @author Anton Bondarev
 * @date 10.10.2025
 */

#include <stdint.h>

#include <drivers/clk.h>
#include <drivers/gpio.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <kernel/irq.h>

#include <config/board_config.h>

#define GPIO_CHIP_ID     OPTION_GET(NUMBER, gpio_chip_id)
#define GPIO_PINS_NUMBER 16

#define GPIOA ((volatile struct gpio_regs *)CONF_GPIO_PORT_A_REGION_BASE)
#define GPIOB ((volatile struct gpio_regs *)CONF_GPIO_PORT_B_REGION_BASE)
#define GPIOC ((volatile struct gpio_regs *)CONF_GPIO_PORT_C_REGION_BASE)

/* clang-format off */
struct gpio_regs {
    volatile uint32_t DR;           /*!< Data                        Address offset: 0x00 */
    volatile uint32_t DDR;          /*!< Direction                   Address offset: 0x04 */
    uint32_t RESERVED0[10];         /*!<                             Address offset: 0x08 */
    volatile uint32_t INTEN;        /*!< Interrupt enable            Address offset: 0x30 */
    volatile uint32_t INTMSK;       /*!< Interrupt mask              Address offset: 0x34 */
    volatile uint32_t INTLVL;       /*!< Interrupt level             Address offset: 0x38 */
    volatile uint32_t INTPOLARITY;  /*!< Interrupt polarity          Address offset: 0x3C */
    volatile uint32_t INTSTAT;      /*!< Interrupt status            Address offset: 0x40 */
    volatile uint32_t INTSTATRAW;   /*!< Raw interrupt status        Address offset: 0x44 */
    volatile uint32_t DEBOUNCE;     /*!< Debounce enable             Address offset: 0x48 */
    volatile uint32_t EOI;          /*!< Clear interrupt             Address offset: 0x4C */
    volatile uint32_t EXT;          /*!< External port               Address offset: 0x50 */
    uint32_t RESERVED1[3];          /*!<                             Address offset: 0x54 */
    volatile uint32_t SYNC;         /*!< Synchronization level       Address offset: 0x60 */
    uint32_t RESERVED2[1];          /*!<                             Address offset: 0x64 */
    volatile uint32_t BOTHEDGE;     /*!< Interrupt Both Edge type    Address offset: 0x68 */
};
/* clang-format on */

static const struct gpio_chip bmcu_gpio_chip;

static inline volatile struct gpio_regs *bmcu_gpio_get_gpio_port(int port) {
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
}

static int bmcu_gpio_setup_mode(unsigned int port, gpio_mask_t pins, uint32_t mode) {
	volatile struct gpio_regs *gpio_regs;
	char *clk_name;

	gpio_regs = bmcu_gpio_get_gpio_port(port);
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
		gpio_regs->DDR &= ~pins;
	}

	if (mode & GPIO_MODE_OUT) {
		gpio_regs->DDR |= pins;
	}

	if (mode & GPIO_MODE_INT_SECTION) {
		// bmcu_gpio_setup_irq(port, pins, mode);
	}

	if (mode & GPIO_MODE_ALT_SECTION) {
		for (int i = 0; i < GPIO_PINS_NUMBER; i++) {
			if (pins & (1 << i)) {
				/* Enable ALTFUNC */
				uint32_t alt = GPIO_MODE_ALT_GET(mode);
				clk_altfunc_enable(port, i, alt);
			}
		}
	}

	return 0;
}

static void bmcu_gpio_set(unsigned int port, gpio_mask_t pins, int level) {
	volatile struct gpio_regs *gpio_regs;

	gpio_regs = bmcu_gpio_get_gpio_port(port);
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

static gpio_mask_t bmcu_gpio_get(unsigned int port, gpio_mask_t pins) {
	volatile struct gpio_regs *gpio_regs;

	gpio_regs = bmcu_gpio_get_gpio_port(port);
	if (gpio_regs == NULL) {
		return -1;
	}

	return gpio_regs->DR & pins;
}

static const struct gpio_chip bmcu_gpio_chip = {
    .setup_mode = bmcu_gpio_setup_mode,
    .get = bmcu_gpio_get,
    .set = bmcu_gpio_set,
    .nports = 3,
    .chip_id = GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&bmcu_gpio_chip);
