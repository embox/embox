/**
 * @file
 * @brief RDC GPIO driver
 *
 * @date 31.03.11
 * @author Nikolay Korotkiy
 */

#include <assert.h>

#include <asm/io.h>
#include <drivers/gpio.h>
#include <embox/unit.h>

#define GPIO_CHIP_ID OPTION_GET(NUMBER, gpio_chip_id)

EMBOX_UNIT_INIT(gpio_init);

#define RDC_CONTROL 0x80003848
#define RDC_DATA    0x8000384c

#define PCI_ADDR_SEL 0xcf8
#define PCI_DATA_REG 0xcfc

#define GPIO_RTCRD (1 << 16) /* Red LED */
#define GPIO_RTCAS (1 << 15) /* Reset button */

static unsigned long g_last_value = 0xffffffff;

static inline void set_control(unsigned long mask) {
	unsigned long tmp;
	/* Select control register */
	out32(RDC_CONTROL, PCI_ADDR_SEL);
	tmp = in32(PCI_DATA_REG);
	/* raise to set GPIO function */
	tmp |= mask;
	out32(tmp, PCI_DATA_REG);
}

static inline void set_data(unsigned long mask) {
	out32(RDC_DATA, PCI_ADDR_SEL);
	g_last_value |= mask;
	out32(g_last_value, PCI_DATA_REG);
}

static inline void clear_data(unsigned long mask) {
	out32(RDC_DATA, PCI_ADDR_SEL);
	g_last_value &= ~mask;
	out32(g_last_value, PCI_DATA_REG);
}

static int rdc_gpio_setup_mode(unsigned int port, gpio_mask_t pins,
    uint32_t mode) {
	if ((mode & GPIO_MODE_OUT_SECTION)
	    && (mode & GPIO_MODE_IN_SECTION)) { /* mode is incorrect */
		return -1;
	}

	if (mode & GPIO_MODE_IN) {
		/* raise logic level to turn off pull-down */
		set_data(pins);
		/* select as GPIO function */
		set_control(pins);
	}
	else if (mode & GPIO_MODE_OUT) {
		set_control(pins);
	}

	return 0;
}

static void rdc_gpio_set(unsigned int port, gpio_mask_t pins, int level) {
	if (level) {
		set_data(pins);
	}
	else {
		clear_data(pins);
	}
}

static gpio_mask_t rdc_gpio_get(unsigned int port, gpio_mask_t pins) {
	unsigned long tmp;
	out32(RDC_DATA, PCI_ADDR_SEL);
	tmp = in32(PCI_DATA_REG);
	return tmp & pins;
}

static const struct gpio_chip rdc_gpio_chip = {
    .setup_mode = rdc_gpio_setup_mode,
    .get = rdc_gpio_get,
    .set = rdc_gpio_set,
    .nports = 1,
    .chip_id = GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&rdc_gpio_chip);

static int gpio_init(void) {
	/* Example: blink led */
	set_control(GPIO_RTCRD);
	out32(RDC_DATA, PCI_ADDR_SEL);
	//	out32(0, PCI_DATA_REG);          // red led on
	//	out32(GPIO_RTCRD, PCI_DATA_REG); // red led off
	return 0;
}
