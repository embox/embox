/**
 * @file
 * @brief
 *
 * @author  Andrew Bursian
 * @date    21.02.2023
 */

#include <stdint.h>
#include <hal/reg.h>

#include <drivers/gpio/gpio_driver.h>

#include <system_k1921vg015.h>

#include <framework/mod/options.h>

#define GPIO_CHIP_ID OPTION_GET(NUMBER,gpio_chip_id)

//#define GPIOA				0x40010000
//#define GPIOB				0x40011000
#define GPIO_OFFSET_DATAOUTSET		0x08
#define GPIO_OFFSET_DATAOUTCLR		0x0C
#define GPIO_OFFSET_DENSET		0x14
#define GPIO_OFFSET_OUTENSET		0x2C
#define GPIO_OFFSET_OUTENCLR		0x30
#define GPIO_OFFSET_ALTFUNCSET		0x34
#define GPIO_OFFSET_ALTFUNCCLR		0x38

#define RCU_HCLKCFG			0x40041100
#define RCU_HCLKCFG_GPIOAEN_mask	0x00000001
#define RCU_HCLKCFG_GPIOBEN_mask	0x00000002
#define RCU_HRSTCFG			0x40041104
#define RCU_HRSTCFG_GPIOAEN_mask	0x00000001
#define RCU_HRSTCFG_GPIOBEN_mask	0x00000002

static int niiet_gpio_clock_setup(unsigned char port) {
	switch (port) {
		case 0:
			RCU->RCU_CGCFGAHB_reg |= RCU_CGCFGAHB_GPIOAEN;
			RCU->RCU_RSTDISAHB_reg |= RCU_RSTDISAHB_GPIOAEN;
		break;
		case 1:
			RCU->RCU_CGCFGAHB_reg |= RCU_CGCFGAHB_GPIOBEN;
			RCU->RCU_RSTDISAHB_reg |= RCU_RSTDISAHB_GPIOBEN;
		break;
#if defined (GPIOC)
		case 2:
			RCU->RCU_CGCFGAHB_reg |= RCU_CGCFGAHB_GPIOCEN;
			RCU->RCU_RSTDISAHB_reg |= RCU_RSTDISAHB_GPIOCEN;
		break;		
#endif /* defined (PORTC) */
		default:
			return -1;
	}

	return 0;
#if 0
	if (port == GPIO_PORT_A) {
		REG32_ORIN (RCU_HCLKCFG, RCU_HCLKCFG_GPIOAEN_mask);
		REG32_ORIN (RCU_HRSTCFG, RCU_HRSTCFG_GPIOAEN_mask);
	} else { // GPIO_PORT_B
		REG32_ORIN (RCU_HCLKCFG, RCU_HCLKCFG_GPIOBEN_mask);
		REG32_ORIN (RCU_HRSTCFG, RCU_HRSTCFG_GPIOBEN_mask);
	}
	return 0;
#endif
}
static inline volatile struct gpio_reg *niiet_gpio_get_gpio(unsigned char port) {
		switch (port) {
		case 0:
			return GPIOA;
		case 1:
			return GPIOB;
#if defined (GPIOC)
		case 2:
			return GPIOC;
		break;		
#endif /* defined (PORTC) */
		default:
			return NULL;
	}

	return 0;
}
static int niiet_gpio_setup_mode(unsigned char port, gpio_mask_t pins, int mode) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = niiet_gpio_get_gpio(port);
	if (gpio_reg == NULL) {
		return -1;
	}
	/* Enable port */
	niiet_gpio_clock_setup(port);

	return 0;
#if 0
	unsigned long GPIO = GPIOA + (GPIOB-GPIOA)*port;
	/* Enable digital functionality */
	/* No definition for this mode in gpio.h */
	REG32_STORE(GPIO + GPIO_OFFSET_DENSET, pins); // set_gpio_digital(GPIO, pins);
	if (mode & GPIO_MODE_IN) {
		REG32_STORE(GPIO + GPIO_OFFSET_OUTENCLR, pins); // set_gpio_mode_input(GPIO, pins); ??? what about altfunc?
		//if (mode & GPIO_MODE_IN_PULL_UP)         set_gpio_mode_pull_up(GPIO, pins);
		//if (mode & GPIO_MODE_IN_PULL_DOWN)        set_gpio_mode_pull_down(GPIO, pins);
		}
	if (mode & GPIO_MODE_OUT) {
		REG32_STORE(GPIO + GPIO_OFFSET_OUTENSET, pins); // set_gpio_mode_output(GPIO, pins);
		//if (mode & GPIO_MODE_OUT_OPEN_DRAIN)    set_gpio_mode_open_drain(GPIO, pins);
		//if (mode & GPIO_MODE_OUT_PUSH_PULL)        set_gpio_mode_push_pull(GPIO, pins);
		}
	if (mode & GPIO_MODE_OUT_ALTERNATE) {
		/* Enable ALTFUNC */
		REG32_STORE(GPIO + GPIO_OFFSET_ALTFUNCSET, pins); // set_gpio_mode_output(GPIO, pins);
		}
	return 0;
#endif
}

static void niiet_gpio_set(unsigned char port, gpio_mask_t pins, char level) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = niiet_gpio_get_gpio(port);
	if (gpio_reg == NULL) {
		return;
	}

#if 0
	if (level) {
		REG32_STORE(GPIOA + (GPIOB-GPIOA)*port + GPIO_OFFSET_DATAOUTSET, pins);
	} else {
		REG32_STORE(GPIOA + (GPIOB-GPIOA)*port + GPIO_OFFSET_DATAOUTCLR, pins);
	}
#endif
}

static gpio_mask_t niiet_gpio_get(unsigned char port, gpio_mask_t pins) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = niiet_gpio_get_gpio(port);
	if (gpio_reg == NULL) {
		return -1;
	}
	return 0;
#if 0
	return REG_LOAD(GPIOA + (GPIOB-GPIOA)*port) & pins;
#endif
}

static struct gpio_chip niiet_gpio_chip = {
	.setup_mode = niiet_gpio_setup_mode,
	.get = niiet_gpio_get,
	.set = niiet_gpio_set,
#if defined (PORTC)
	.nports = 3
#else
	.nports = 2
#endif
};

GPIO_CHIP_DEF(&niiet_gpio_chip);

