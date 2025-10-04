/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    17.07.2024
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/gpio.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <kernel/irq.h>

#define GPIO_CHIP_ID   OPTION_GET(NUMBER, chip_id)
#define GPIO_BASE_ADDR OPTION_GET(NUMBER, base_addr)
#define GPIO_PORTA_IRQ OPTION_GET(NUMBER, porta_irq)
#define GPIO_PORTB_IRQ OPTION_GET(NUMBER, portb_irq)
#define GPIO_PORTC_IRQ OPTION_GET(NUMBER, portc_irq)
#define GPIO_PORTD_IRQ OPTION_GET(NUMBER, portd_irq)
#define GPIO_PORTE_IRQ OPTION_GET(NUMBER, porte_irq)
#define GPIO_PORTF_IRQ OPTION_GET(NUMBER, portf_irq)

#define GPIO_NPORTS 6
#define GPIO_NPINS  8

#define REG_ODR 0x00 /* (RW) Output Data Register */
#define REG_BSR 0x01 /* (RW) Bit Set Register */
#define REG_BRR 0x02 /* (RW) Bit Reset Register */
#define REG_BIR 0x03 /* (RW) Bit Inverse Register */
#define REG_IDR 0x04 /* (RO) Input Data Register */
#define REG_ISR 0x05 /* (RO) Interrupt Status Register */

#define REG_CONF(n)       (0x08 + n) /* (n: 0..7) */
#define REG_FILTER(n)     (0x10 + n) /* (n: 0..7) */
#define REG_EVENT(n)      (0x18 + n) /* (n: 0..7) */
#define REG_EVENT_TIME(n) (0x20 + n) /* (n: 0..63) */

#define REG_CONF_IN  0b00 /* (RW) Input Mode */
#define REG_CONF_IPU 0b01 /* (RW) Input Pull-up Mode */
#define REG_CONF_OUT 0b10 /* (RW) Output Mode */

#define REG_EVENT_ES (1 << 0) /* (RC) Event Status */
#define REG_EVENT_L0 (1 << 1) /* (RW) Level 0 */
#define REG_EVENT_L1 (2 << 1) /* (RW) Level 1 */
#define REG_EVENT_RE (3 << 1) /* (RW) Rising Edge */
#define REG_EVENT_FE (4 << 1) /* (RW) Falling Edge */
#define REG_EVENT_RF (5 << 1) /* (RW) Rising and Falling Edge */
#define REG_EVENT_IE (1 << 4) /* (RW) Interrupt Enable */

#define GPIO_REG_ADDR(port, reg)       (GPIO_BASE_ADDR + 0x80 * (port) + (reg))
#define GPIO_REG_LOAD(port, reg)       REG8_LOAD(GPIO_REG_ADDR(port, reg))
#define GPIO_REG_ORIN(port, reg, val)  REG8_ORIN(GPIO_REG_ADDR(port, reg), val)
#define GPIO_REG_STORE(port, reg, val) REG8_STORE(GPIO_REG_ADDR(port, reg), val)
#define GPIO_REG_ANDIN(port, reg, val) REG8_ANDIN(GPIO_REG_ADDR(port, reg), val)
#define GPIO_REG_CLEAR(port, reg, val) REG8_CLEAR(GPIO_REG_ADDR(port, reg), val)

static int sc64_gpio_setup_mode(unsigned int port, gpio_mask_t pins,
    uint32_t mode) {
	uint8_t irq_mode;
	uint8_t event;
	uint8_t conf;
	int i;

	if (mode & (GPIO_MODE_IN_SECTION | GPIO_MODE_OUT_SECTION)) {
		if (mode & GPIO_MODE_OUT_SECTION) {
			conf = REG_CONF_OUT;
		}
		else if (mode & GPIO_MODE_IN_PULL_UP) {
			conf = REG_CONF_IPU;
		}
		else {
			conf = REG_CONF_IN;
		}
		for (i = 0; i < GPIO_NPINS; i++) {
			if (pins & (1 << i)) {
				GPIO_REG_STORE(port, REG_CONF(i), conf);
			}
		}
	}

	if (mode & (GPIO_MODE_INT_SECTION)) {
		irq_mode = 0;

		if (mode & GPIO_MODE_INT_MODE_LEVEL0) {
			irq_mode = REG_EVENT_IE | REG_EVENT_L0;
		}
		else if (mode & GPIO_MODE_INT_MODE_LEVEL1) {
			irq_mode = REG_EVENT_IE | REG_EVENT_L1;
		}
		else if ((mode & GPIO_MODE_INT_MODE_RISING)
		         && (mode & GPIO_MODE_INT_MODE_FALLING)) {
			irq_mode = REG_EVENT_IE | REG_EVENT_RF;
		}
		else if (mode & GPIO_MODE_INT_MODE_RISING) {
			irq_mode = REG_EVENT_IE | REG_EVENT_RE;
		}
		else if (mode & GPIO_MODE_INT_MODE_FALLING) {
			irq_mode = REG_EVENT_IE | REG_EVENT_FE;
		}

		for (i = 0; i < GPIO_NPINS; i++) {
			if (pins & (1 << i)) {
				event = GPIO_REG_LOAD(port, REG_EVENT(i));
				if (irq_mode) {
					event &= ~(uint8_t)0b1110;
					event |= irq_mode;
				}
				if (mode & GPIO_MODE_IN_INT_EN) {
					event |= REG_EVENT_IE;
				}
				if (mode & GPIO_MODE_IN_INT_DIS) {
					event &= ~(uint8_t)REG_EVENT_IE;
				}
				event |= REG_EVENT_ES;
				GPIO_REG_STORE(port, REG_EVENT(i), event);
			}
		}
	}

	return 0;
}

static gpio_mask_t sc64_gpio_get(unsigned int port, gpio_mask_t pins) {
	return GPIO_REG_LOAD(port, REG_IDR) & pins;
}

static void sc64_gpio_set(unsigned int port, gpio_mask_t pins, int level) {
	GPIO_REG_STORE(port, level ? REG_BSR : REG_BRR, pins);
}

static const struct gpio_chip sc64_gpio_chip = {
    .setup_mode = sc64_gpio_setup_mode,
    .get = sc64_gpio_get,
    .set = sc64_gpio_set,
    .nports = GPIO_NPORTS,
    .chip_id = GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&sc64_gpio_chip);

static unsigned int sc64_gpio_irqs[GPIO_NPORTS] = {
    GPIO_PORTA_IRQ,
    GPIO_PORTB_IRQ,
    GPIO_PORTC_IRQ,
    GPIO_PORTD_IRQ,
    GPIO_PORTE_IRQ,
    GPIO_PORTF_IRQ,
};

irq_return_t sc64_gpio_irq_handler(unsigned int irq_nr, void *gpio) {
	gpio_mask_t pins;
	uint8_t port;
	int i;

	for (i = 0; i < GPIO_NPORTS; i++) {
		if (sc64_gpio_irqs[i] == irq_nr) {
			port = i;
			break;
		}
	}
	assert(i != GPIO_NPORTS);

	pins = GPIO_REG_LOAD(port, REG_ISR);

	gpio_handle_irq(&sc64_gpio_chip, port, pins);

	for (i = 0; i < GPIO_NPINS; i++) {
		if (pins & (1 << i)) {
			GPIO_REG_ORIN(port, REG_EVENT(i), REG_EVENT_ES);
		}
	}

	return IRQ_HANDLED;
}

EMBOX_UNIT_INIT(sc64_gpio_init);

static int sc64_gpio_init(void) {
	int err;
	int i;

	for (i = 0; i < GPIO_NPORTS; i++) {
		err = irq_attach(sc64_gpio_irqs[i], sc64_gpio_irq_handler, 0, NULL,
		    "sc64-gpio");
		if (err) {
			return err;
		}
	}

	return 0;
}
