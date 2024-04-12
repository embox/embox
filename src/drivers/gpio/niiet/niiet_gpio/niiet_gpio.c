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

#include <drivers/clk/k1921vg015_rcu.h>

#include <system_k1921vg015.h>

#include <framework/mod/options.h>

#define GPIO_CHIP_ID       OPTION_GET(NUMBER,gpio_chip_id)
#define GPIO_PINS_NUMBER   16

#define GPIOA                ((volatile struct gpio_reg *) GPIOA_BASE)
#define GPIOB                ((volatile struct gpio_reg *) GPIOB_BASE)
#define GPIOC                ((volatile struct gpio_reg *) GPIOC_BASE)

struct gpio_reg {
    uint32_t 	GPIO_DATA_reg;        /* 0x00 */
    uint32_t 	GPIO_DATAOUT_reg;     /* 0x04 */
    uint32_t 	GPIO_DATAOUTSET_reg;  /* 0x08 */
    uint32_t 	GPIO_DATAOUTCLR_reg;  /* 0x0C */
    uint32_t 	GPIO_DATAOUTTGL_reg;  /* 0x10 */
    uint32_t 	GPIO_DENSET_reg;      /* 0x14 */
    uint32_t 	GPIO_DENCLR_reg;      /* 0x18 */
    uint32_t 	GPIO_INMODE_reg;      /* 0x1C */
    uint32_t 	GPIO_PULLMODE_reg;    /* 0x20 */
    uint32_t 	GPIO_OUTMODE_reg;     /* 0x24 */
    uint32_t 	GPIO_DRIVEMODE_reg;   /* 0x28 */
    uint32_t 	GPIO_OUTENSET_reg;    /* 0x2C */
    uint32_t 	GPIO_OUTENCLR_reg;    /* 0x30 */
    uint32_t 	GPIO_ALTFUNCSET_reg;  /* 0x34 */
    uint32_t 	GPIO_ALTFUNCCLR_reg;  /* 0x38 */
    uint32_t 	GPIO_ALTFUNCNUM_reg;  /* 0x3C */
    uint32_t reserved2[1];
    uint32_t 	GPIO_SYNCSET_reg;
    uint32_t 	GPIO_SYNCCLR_reg;
    uint32_t 	GPIO_QUALSET_reg;
    uint32_t 	GPIO_QUALCLR_reg;
    uint32_t 	GPIO_QUALMODESET_reg;
    uint32_t 	GPIO_QUALMODECLR_reg;
    uint32_t 	GPIO_QUALSAMPLE_reg;
    uint32_t 	GPIO_INTENSET_reg;
    uint32_t 	GPIO_INTENCLR_reg;
    uint32_t 	GPIO_INTTYPESET_reg;
    uint32_t 	GPIO_INTTYPECLR_reg;
    uint32_t 	GPIO_INTPOLSET_reg;
    uint32_t 	GPIO_INTPOLCLR_reg;
    uint32_t 	GPIO_INTEDGESET_reg;
    uint32_t 	GPIO_INTEDGECLR_reg;
    uint32_t 	GPIO_INTSTATUS_reg;
    uint32_t 	GPIO_DMAREQSET_reg;
    uint32_t 	GPIO_DMAREQCLR_reg;
    uint32_t 	GPIO_ADCSOCSET_reg;
    uint32_t 	GPIO_ADCSOCCLR_reg;
    uint32_t reserved3[2];
    uint32_t 	GPIO_LOCKKEY_reg;
    uint32_t 	GPIO_LOCKSET_reg;
    uint32_t 	GPIO_LOCKCLR_reg;
};

#if 0
#define GPIOA				0x40010000
#define GPIOB				0x40011000
#endif

static inline volatile struct gpio_reg *niiet_gpio_get_gpio_port(unsigned char port) {
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

	gpio_reg = niiet_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return -1;
	}
	/* Enable port */
	niiet_gpio_clock_setup(port);

	gpio_reg->GPIO_DENSET_reg |= pins;

	if (mode & GPIO_MODE_IN) {
		gpio_reg->GPIO_OUTENCLR_reg |= pins;
	}
	if (mode & GPIO_MODE_OUT) {
		gpio_reg->GPIO_OUTENSET_reg |= pins;
	
	}
	if (mode & GPIO_MODE_OUT_ALTERNATE) {
		/* Enable ALTFUNC */
		gpio_reg->GPIO_ALTFUNCSET_reg |= pins;
	}
	if (GPIO_GET_ALTERNATE(mode) != 0){
		for (int i = 0; i < GPIO_PINS_NUMBER; i++){ // TODO use bit_ctz()
			if (pins & (1 << i)) {
				gpio_reg->GPIO_ALTFUNCNUM_reg |= GPIO_GET_ALTERNATE(mode) << i*2;
			}
		}
	}

	return 0;
}

static void niiet_gpio_set(unsigned char port, gpio_mask_t pins, char level) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = niiet_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return;
	}

	if (level) {
		gpio_reg->GPIO_DATAOUTSET_reg |= pins;
	} else {		
		gpio_reg->GPIO_DATAOUTCLR_reg |= pins;
	}
}

static gpio_mask_t niiet_gpio_get(unsigned char port, gpio_mask_t pins) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = niiet_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return -1;
	}

	return gpio_reg->GPIO_DATA_reg & pins;
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

