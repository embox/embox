/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 26.02.25
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
#include <lib/libds/array.h>
#include <util/log.h>

#define GPIO_CHIP_ID    OPTION_GET(NUMBER, chip_id)
#define GPIO_BASE_ADDR  OPTION_GET(NUMBER, gpio_base_addr)
#define OUTER_BASE_ADDR OPTION_GET(NUMBER, outer_base_addr)

#define GPIO_PORTA2_IRQ OPTION_GET(NUMBER, porta2_irq)
#define GPIO_PORTA3_IRQ OPTION_GET(NUMBER, porta3_irq)
#define GPIO_PORTB2_IRQ OPTION_GET(NUMBER, portb2_irq)
#define GPIO_PORTB3_IRQ OPTION_GET(NUMBER, portb3_irq)
#define GPIO_OUTER0_IRQ OPTION_GET(NUMBER, outer0_irq)
#define GPIO_OUTER1_IRQ OPTION_GET(NUMBER, outer1_irq)

#define GPIO_NPORTS 4
#define GPIO_NPINS  8
#define GPIO_NIRQS  6

#define REG_SLR 0x00 /* (WO) Set Latch Register */
#define REG_IDR 0x00 /* (RO) Input Data Register */
#define REG_CLR 0x01 /* (WO) Clear Latch Register */
#define REG_RLR 0x01 /* (RO) Read Latch Register */
#define REG_DIR 0x02 /* (RW) Direction Register */
#define REG_AFR 0x03 /* (RW) Additional Function Register */
#define REG_ISR 0x04 /* (RW) Interrupt Setup Register */
#define REG_WLR 0x05 /* (WO) Write Latch Register */

#define GPIO_IRQ_HIGH    0
#define GPIO_IRQ_LOW     1
#define GPIO_IRQ_RISING  2
#define GPIO_IRQ_FALLING 3

#define GPIO_REG_ADDR(port, reg) (GPIO_BASE_ADDR + 0x8 * (port) + (reg))
#define OUTER_REG_ADDR(reg)      (OUTER_BASE_ADDR + (reg))

#define GPIO_REG_LOAD(port, reg)       REG8_LOAD(GPIO_REG_ADDR(port, reg))
#define GPIO_REG_ORIN(port, reg, val)  REG8_ORIN(GPIO_REG_ADDR(port, reg), val)
#define GPIO_REG_STORE(port, reg, val) REG8_STORE(GPIO_REG_ADDR(port, reg), val)
#define GPIO_REG_ANDIN(port, reg, val) REG8_ANDIN(GPIO_REG_ADDR(port, reg), val)
#define GPIO_REG_CLEAR(port, reg, val) REG8_CLEAR(GPIO_REG_ADDR(port, reg), val)

#define OUTER_REG_LOAD(reg)       REG8_LOAD(OUTER_REG_ADDR(reg))
#define OUTER_REG_ORIN(reg, val)  REG8_ORIN(OUTER_REG_ADDR(reg), val)
#define OUTER_REG_STORE(reg, val) REG8_STORE(OUTER_REG_ADDR(reg), val)
#define OUTER_REG_ANDIN(reg, val) REG8_ANDIN(OUTER_REG_ADDR(reg), val)
#define OUTER_REG_CLEAR(reg, val) REG8_CLEAR(OUTER_REG_ADDR(reg), val)

/* Interrupt Setup Registers */
static const struct {
	uintptr_t reg;
	unsigned int port;
	unsigned int pins[2];
} gpio_isr_regs[] = {
    {GPIO_REG_ADDR(GPIO_PORT_A, REG_ISR), GPIO_PORT_A, {1 << 2, 1 << 3}},
    {GPIO_REG_ADDR(GPIO_PORT_B, REG_ISR), GPIO_PORT_B, {1 << 2, 1 << 3}},
    {OUTER_REG_ADDR(0), GPIO_PORT_A, {1 << 0, 1 << 1}},
    {OUTER_REG_ADDR(1), GPIO_PORT_A, {1 << 4, 1 << 5}},
    {OUTER_REG_ADDR(2), GPIO_PORT_A, {1 << 6, 1 << 7}},
    {OUTER_REG_ADDR(3), GPIO_PORT_B, {1 << 6, 1 << 0}},
};

static int sc64_gpio_setup_mode(unsigned int port, gpio_mask_t pins,
    uint32_t mode) {
	int irq_mode;
	int i, j;
	uint8_t isr;

	if (mode & GPIO_MODE_IN_SECTION) {
		GPIO_REG_CLEAR(port, REG_DIR, pins);
	}

	if (mode & GPIO_MODE_OUT_SECTION) {
		GPIO_REG_ORIN(port, REG_DIR, pins);
	}

	if (mode & GPIO_MODE_ALT_SECTION)
		switch (GPIO_MODE_ALT_GET(mode)) {
		case 0:
			GPIO_REG_CLEAR(port, REG_AFR, pins);
			break;
		case 1:
			GPIO_REG_ORIN(port, REG_AFR, pins);
			break;
		}

	if (mode & GPIO_MODE_INT_SECTION) {
		irq_mode = -1;

		if (mode & GPIO_MODE_INT_MODE_LEVEL0) {
			irq_mode = GPIO_IRQ_LOW;
		}
		else if (mode & GPIO_MODE_INT_MODE_LEVEL1) {
			irq_mode = GPIO_IRQ_HIGH;
		}
		else if (mode & GPIO_MODE_INT_MODE_RISING) {
			irq_mode = GPIO_IRQ_RISING;
		}
		else if (mode & GPIO_MODE_INT_MODE_FALLING) {
			irq_mode = GPIO_IRQ_FALLING;
		}

		for (i = 0; i < ARRAY_SIZE(gpio_isr_regs); i++) {
			if (gpio_isr_regs[i].port == port) {
				for (j = 0; j < 2; j++) {
					if (gpio_isr_regs[i].pins[j] & pins) {
						isr = REG8_LOAD(gpio_isr_regs[i].reg);
						if (irq_mode != -1) {
							isr &= ~(uint8_t)(0b11 << (4 + 2 * j));
							isr |= (irq_mode << (4 + 2 * j));
						}
						if (mode & GPIO_MODE_IN_INT_EN) {
							isr |= 0b0100 << j;
						}
						if (mode & GPIO_MODE_IN_INT_DIS) {
							isr &= ~(uint8_t)(0b100 << j);
						}
						isr |= (0b01 << j);
						REG8_STORE(gpio_isr_regs[i].reg, isr);
					}
				}
			}
		}
	}

	return 0;
}

static gpio_mask_t sc64_gpio_get(unsigned int port, gpio_mask_t pins) {
	return GPIO_REG_LOAD(port, REG_IDR) & pins;
}

static void sc64_gpio_set(unsigned int port, gpio_mask_t pins, int level) {
	GPIO_REG_STORE(port, level ? REG_SLR : REG_CLR, pins);
}

static const struct gpio_chip sc64_gpio_chip = {
    .setup_mode = sc64_gpio_setup_mode,
    .get = sc64_gpio_get,
    .set = sc64_gpio_set,
    .nports = GPIO_NPORTS,
    .chip_id = GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&sc64_gpio_chip);

irq_return_t sc64_gpio_irq_handler(unsigned int irq_nr, void *gpio) {
	gpio_mask_t pins;
	uint8_t isr;
	int i;

	for (i = 0; i < ARRAY_SIZE(gpio_isr_regs); i++) {
		pins = 0;
		isr = REG8_LOAD(gpio_isr_regs[i].reg);
		if (isr & 0b0101) {
			pins |= gpio_isr_regs[i].pins[0];
		}
		if (isr & 0b1010) {
			pins |= gpio_isr_regs[i].pins[1];
		}
		if (pins) {
			gpio_handle_irq(&sc64_gpio_chip, gpio_isr_regs[i].port, pins);
			REG8_STORE(gpio_isr_regs[i].reg, isr);
		}
	}

	return IRQ_HANDLED;
}

static const unsigned int gpio_irq_nums[GPIO_NIRQS] = {
    GPIO_PORTA2_IRQ,
    GPIO_PORTA3_IRQ,
    GPIO_PORTB2_IRQ,
    GPIO_PORTB3_IRQ,
    GPIO_OUTER0_IRQ,
    GPIO_OUTER1_IRQ,
};

EMBOX_UNIT_INIT(sc64_gpio_init);

static int sc64_gpio_init(void) {
	int err;
	int i;

	for (i = 0; i < GPIO_NIRQS; i++) {
		err = irq_attach(gpio_irq_nums[i], sc64_gpio_irq_handler, 0, NULL,
		    "sc64-gpio");
		if (err) {
			return err;
		}
	}

	return 0;
}
