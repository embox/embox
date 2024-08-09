/**
 * @file
 * @brief
 *
 * @author  Andrew Bursian
 * @date    21.02.2023
 */

#include <stdint.h>
#include <hal/reg.h>
#include <kernel/irq.h>

#include <drivers/gpio/gpio_driver.h>

#include <drivers/clk/k1921vg015_rcu.h>

#include <config/board_config.h>

#include <framework/mod/options.h>

#define GPIO_CHIP_ID       OPTION_GET(NUMBER,gpio_chip_id)
#define GPIO_PINS_NUMBER   16

#define GPIOA     ((volatile struct gpio_reg *)CONF_GPIO_PORT_A_REGION_BASE)
#define GPIOB     ((volatile struct gpio_reg *)CONF_GPIO_PORT_B_REGION_BASE)
#if defined(CONF_GPIO_PORT_C_REGION_BASE)
# define GPIOC     ((volatile struct gpio_reg *)CONF_GPIO_PORT_C_REGION_BASE)
#endif /* defined(CONF_GPIO_PORT_C_REGION_BASE) */

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
    uint32_t 	GPIO_SYNCSET_reg;	  /* 0x44 */
    uint32_t 	GPIO_SYNCCLR_reg;     /* 0x48 */
    uint32_t 	GPIO_QUALSET_reg;     /* 0x4C */
    uint32_t 	GPIO_QUALCLR_reg;     /* 0x50 */
    uint32_t 	GPIO_QUALMODESET_reg; /* 0x54 */
    uint32_t 	GPIO_QUALMODECLR_reg; /* 0x58 */
    uint32_t 	GPIO_QUALSAMPLE_reg;  /* 0x5C */
    uint32_t 	GPIO_INTENSET_reg;    /* 0x60 */
    uint32_t 	GPIO_INTENCLR_reg;    /* 0x64 */
    uint32_t 	GPIO_INTTYPESET_reg;  /* 0x68 */
    uint32_t 	GPIO_INTTYPECLR_reg;  /* 0x6C */
    uint32_t 	GPIO_INTPOLSET_reg;   /* 0x70 */
    uint32_t 	GPIO_INTPOLCLR_reg;   /* 0x74 */
    uint32_t 	GPIO_INTEDGESET_reg;  /* 0x78 */
    uint32_t 	GPIO_INTEDGECLR_reg;  /* 0x7C */
    uint32_t 	GPIO_INTSTATUS_reg;   /* 0x80 */
    uint32_t 	GPIO_DMAREQSET_reg;   /* 0x84 */
    uint32_t 	GPIO_DMAREQCLR_reg;   /* 0x88 */
    uint32_t 	GPIO_ADCSOCSET_reg;   /* 0x8C */
    uint32_t 	GPIO_ADCSOCCLR_reg;   /* 0x90 */
    uint32_t reserved3[2];
    uint32_t 	GPIO_LOCKKEY_reg;     /* 0x9C */
    uint32_t 	GPIO_LOCKSET_reg;     /* 0xA0 */
    uint32_t 	GPIO_LOCKCLR_reg;     /* 0xA4 */
};

static struct gpio_chip niiet_gpio_chip;

static inline void
niiet_gpio_irq_conf_type(volatile struct gpio_reg *gpio_reg, uint32_t mask, int type) {
	if (1 == type) {
		gpio_reg->GPIO_INTTYPESET_reg = mask;
	} else {
		gpio_reg->GPIO_INTTYPECLR_reg = mask;
	}
}

static inline void
niiet_gpio_irq_conf_pol(volatile struct gpio_reg *gpio_reg, uint32_t mask, int pol) {
	if (1 == pol) {
		gpio_reg->GPIO_INTPOLSET_reg = mask;
	} else {
		gpio_reg->GPIO_INTPOLCLR_reg = mask;
	}
}

static inline void
niiet_gpio_irq_conf_edge(volatile struct gpio_reg *gpio_reg, uint32_t mask, int edge) {
	if (1 == edge) {
		gpio_reg->GPIO_INTEDGESET_reg = mask;
	} else {
		gpio_reg->GPIO_INTEDGECLR_reg = mask;
	}
}

static inline void 
niiet_gpio_irq_en(volatile struct gpio_reg *gpio_reg, uint32_t mask) {
	gpio_reg->GPIO_INTENSET_reg = mask;
}

static inline void 
niiet_gpio_irq_dis(volatile struct gpio_reg *gpio_reg, uint32_t mask) {
	gpio_reg->GPIO_INTENCLR_reg = mask;
}

static inline uint32_t 
niiet_gpio_irq_get_status(volatile struct gpio_reg *gpio_reg) {
	return gpio_reg->GPIO_INTSTATUS_reg;
}

static inline void 
niiet_gpio_irq_clear_status(volatile struct gpio_reg *gpio_reg, uint32_t mask) {
	gpio_reg->GPIO_INTSTATUS_reg = mask;
}

static inline volatile struct gpio_reg *niiet_gpio_get_gpio_port(unsigned char port) {
		switch (port) {
		case 0:
			return GPIOA;
		case 1:
			return GPIOB;
#if defined (CONF_GPIO_PORT_C_REGION_BASE)
		case 2:
			return GPIOC;
		break;
#endif /* defined (PORTC) */
		default:
			return NULL;
	}

	return 0;
}

irq_return_t niiet_gpio_irq_handler(unsigned int irq_nr, void *gpio) {
	uint32_t mask = 0;
	uint8_t port_num = 0;
	volatile struct gpio_reg *gpio_reg = gpio;
	
	mask = niiet_gpio_irq_get_status(gpio_reg);

	niiet_gpio_irq_clear_status(gpio_reg, mask);

	gpio_handle_irq(&niiet_gpio_chip, port_num, mask);

	return IRQ_HANDLED;
}

static int niiet_gpio_setup_irq(int port, uint32_t mask, int mode) {
	int res;
	volatile struct gpio_reg *gpio_reg;
	int type;
	int edge;
	int pol;

	gpio_reg = niiet_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return -1;
	}

	res = irq_attach(0, niiet_gpio_irq_handler, 0, (void*)gpio_reg, "GPIO Irq");
	if (res < 0) {
		return res;
	}

	if ( (mode & GPIO_MODE_INT_MODE_LEVEL0)
			|| (mode & GPIO_MODE_INT_MODE_LEVEL1) ) {
		type = 0;
	} else {
		type = 1;
	}
	
	if ((mode & GPIO_MODE_INT_MODE_RISING)
			&& (mode & GPIO_MODE_INT_MODE_FALLING)) {
		edge = 1;
	} else {
		edge = 0;
	}

	if ((mode & GPIO_MODE_INT_MODE_RISING)
			|| (mode & GPIO_MODE_INT_MODE_LEVEL1)) {
		pol = 1;
	} else {
		pol = 0;
	}

	niiet_gpio_irq_conf_pol(gpio_reg, mask, pol);
	niiet_gpio_irq_conf_edge(gpio_reg, mask, edge);
	niiet_gpio_irq_conf_type(gpio_reg, mask, type);

	if (mode & GPIO_MODE_IN_INT_EN) {
		niiet_gpio_irq_en(gpio_reg, mask);
	}

	return res;
}

static int niiet_gpio_setup_mode(unsigned char port, gpio_mask_t pins, int mode) {
	volatile struct gpio_reg *gpio_reg;
	char *clk_name;

	gpio_reg = niiet_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return -1;
	}

	switch(port) {
		case 0:
		clk_name = CONF_GPIO_PORT_A_CLK_ENABLE();
		break;
		case 1:
		clk_name = CONF_GPIO_PORT_B_CLK_ENABLE();
		break;
		case 2:
#if defined CONF_GPIO_PORT_C_CLK_ENABLE
		clk_name = CONF_GPIO_PORT_C_CLK_ENABLE();
#endif /* defined CONF_GPIO_PORT_C_CLK_ENABLE */
		break;
		default:
		return -1;
	}

	/* Enable port */
	clk_enable(clk_name);

	gpio_reg->GPIO_DENSET_reg |= pins;

	if (mode & GPIO_MODE_IN) {
		gpio_reg->GPIO_OUTENCLR_reg |= pins;
	}

	if (mode & GPIO_MODE_IN_PULL_UP) {
		gpio_reg->GPIO_PULLMODE_reg |= pins;
	}

	if (mode & GPIO_MODE_OUT) {
		gpio_reg->GPIO_OUTENSET_reg |= pins;
	}

	if (mode & GPIO_MODE_INT_SECTION) {
		niiet_gpio_setup_irq(port, pins, mode);
	}

	if (mode & GPIO_MODE_OUT_ALTERNATE) {
		/* Enable ALTFUNC */
		gpio_reg->GPIO_ALTFUNCSET_reg |= pins;

		for (int i = 0; i < GPIO_PINS_NUMBER; i++) {			
			if (pins & (1 << i)) {
				uint32_t alt = GPIO_GET_ALTERNATE(mode);

				gpio_reg->GPIO_ALTFUNCNUM_reg &= ~(0x3 << i * 2);
				gpio_reg->GPIO_ALTFUNCNUM_reg |= alt << i * 2;
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
#if defined (CONF_GPIO_PORT_C_REGION_BASE)
	.nports = 3
#else
	.nports = 2
#endif
};

GPIO_CHIP_DEF(&niiet_gpio_chip);

STATIC_IRQ_ATTACH(GPIO_IRQ, gpio_irq_handler, NULL);


