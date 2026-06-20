/**
 * @file imx.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 13.06.2017
 */

 #include <util/log.h>

#include <drivers/common/memory.h>
#include <drivers/gpio.h>

#include <hal/reg.h>

#include <kernel/irq.h>

#include <embox/unit.h>

#define GPIO_CHIP_ID OPTION_GET(NUMBER, gpio_chip_id)
#define GPIO_PORTS   OPTION_GET(NUMBER, gpio_ports)
#define BASE_ADDR(n) ((OPTION_GET(NUMBER, base_addr)) + (n)*0x4000)
#define IRQ_NUM(n)   ((OPTION_GET(NUMBER, irq_num)) + (n) * 2)

#define GPIO_DR(n)       (BASE_ADDR(n) + 0x00)
#define GPIO_GDIR(n)     (BASE_ADDR(n) + 0x04)
#define GPIO_PSR(n)      (BASE_ADDR(n) + 0x08)
#define GPIO_ICR1(n)     (BASE_ADDR(n) + 0x0C)
#define GPIO_ICR2(n)     (BASE_ADDR(n) + 0x10)
#define GPIO_IMR(n)      (BASE_ADDR(n) + 0x14)
#define GPIO_ISR(n)      (BASE_ADDR(n) + 0x18)
#define GPIO_EDGE_SEL(n) (BASE_ADDR(n) + 0x1C)

struct imx_gpio_regs {
	volatile uint32_t DR;
	volatile uint32_t GDIR;
	volatile uint32_t PSR;
	volatile uint32_t ICR1;
	volatile uint32_t ICR2;
	volatile uint32_t IMR;
	volatile uint32_t ISR;
	volatile uint32_t EDGE_SEL;
};

#define GPIO_DIR_IN  0
#define GPIO_DIR_OUT 1

#define GPIO_IMX_ICR_LOW_LEVEL    0x0
#define GPIO_IMX_ICR_HIGH_LEVEL   0x1
#define GPIO_IMX_ICR_RISING_EDGE  0x2
#define GPIO_IMX_ICR_FALLING_EDGE 0x3

EMBOX_UNIT_INIT(imx_gpio_init);

static const struct gpio_chip imx_gpio_chip;

static inline int imp_gpio_irq_to_port(int irq_num) {
	return (irq_num - IRQ_NUM(0)) / 2;
}

static irq_return_t imx_gpio_irq_handler(unsigned int irq_nr, void *gpio) {
	uint32_t mask = 0;
	int port;

	(void) gpio;

	port = imp_gpio_irq_to_port(irq_nr);
	mask = REG32_LOAD(GPIO_PSR(port));

	REG32_ORIN(GPIO_ISR(port), REG32_LOAD(GPIO_IMR(port)));
	gpio_handle_irq(irq_nr, &imx_gpio_chip, (uint8_t)port, mask);

	return IRQ_HANDLED;
}

static int imx_gpio_init(void) {
	for (int i = 0; i <= GPIO_PORTS; i++) {
		int res;

		log_debug("GPIO%d base address=%p", i, BASE_ADDR(i));

		/* 0 .. 15 */
		res = irq_attach(IRQ_NUM(i), imx_gpio_irq_handler, 0, 
										(void*)&imx_gpio_chip, "GPIO");
		if (res < 0) {
			log_error("couldn't attach irq %d", IRQ_NUM(i));

			return res;
		}
		/* 16 .. 31 */
		res = irq_attach(IRQ_NUM(i) + 1, imx_gpio_irq_handler, 0,
										(void*)&imx_gpio_chip, "GPIO");
		if (res < 0) {
			log_error("couldn't attach irq %d", IRQ_NUM(i) + 1);
			return res;
		}
	}

	return 0;
}

static int imx_gpio_setup_irq(int port, uint32_t mask, uint32_t mode) {
	uint32_t icr_val = 0;
	int i = 0;

	if (mode & GPIO_MODE_INT_LEVEL0) {
		icr_val = GPIO_IMX_ICR_LOW_LEVEL;
	} else if (mode & GPIO_MODE_INT_LEVEL1) {
		icr_val = GPIO_IMX_ICR_HIGH_LEVEL;
	} else if (mode & GPIO_MODE_INT_RISING) {
		icr_val = GPIO_IMX_ICR_RISING_EDGE;
	} else if (mode & GPIO_MODE_INT_FALLING) {
		icr_val = GPIO_IMX_ICR_FALLING_EDGE;
	}
	for (i = 0; i < 16; i ++) {
		if (mask & (1 << i)) {
			uint32_t tmp;
			tmp = REG32_LOAD(GPIO_ICR1(port));
			tmp &= ~((uint32_t)3 << (i * 2));
			tmp |= icr_val << (i * 2);
			REG32_STORE(GPIO_ICR1(port), tmp);
		}
	}
	for (i = 0; i < 16; i ++) {
		if (mask & (1 << (i + 16))) {
			uint32_t tmp;
			tmp = REG32_LOAD(GPIO_ICR2(port));
			tmp &= ~((uint32_t)3 << (i * 2));
			tmp |= icr_val << (i * 2);
			REG32_STORE(GPIO_ICR2(port), tmp);
		}
	}
	if (mode & GPIO_MODE_INT_RISING_FALLING) {
		REG32_ORIN(GPIO_EDGE_SEL(port), mask);
	}
	REG32_ORIN(GPIO_ISR(port), mask);
	REG32_ORIN(GPIO_IMR(port), mask);
	return 0;
}

static int imx_gpio_setup_mode(unsigned int port, gpio_mask_t mask,
    uint32_t mode) {
	uint32_t val = 0;
	uint32_t tmp;

	log_debug("Set GPIO%d;mask=0x%08x;mode=%d", port, mask, mode);

	if( mode & GPIO_MODE_IN) {
		val = ~mask;

	} else if ( mode & GPIO_MODE_OUT) {
		val = mask;
	}

	if (mode & GPIO_MODE_INT_SECTION) {
		return imx_gpio_setup_irq(port, mask, mode);
	}

	tmp = REG32_LOAD(GPIO_GDIR(port));
	tmp &= ~mask;
	tmp |= val;
	REG32_STORE(GPIO_GDIR(port), tmp);

	return 0;
}

static void imx_gpio_set(unsigned int port, gpio_mask_t mask, int level) {
	log_debug("set level %d for GPIO#%d 0x%08x", level, port, mask);

	if (level == 0) {
		REG32_CLEAR(GPIO_DR(port), mask);
	}
	else {
		REG32_ORIN(GPIO_DR(port), mask);
	}
}

gpio_mask_t imx_gpio_get(unsigned int port, gpio_mask_t mask) {
	int ret = REG32_LOAD(GPIO_PSR(port)) & mask;

	log_debug("get level for GPIO#%d 0x%08x=0x%08x", port, mask, ret);

	return ret;
}


static const struct gpio_chip imx_gpio_chip = {
    .setup_mode = imx_gpio_setup_mode,
    .get = imx_gpio_get,
    .set = imx_gpio_set,
    .nports = GPIO_PORTS,
    .chip_id = GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&imx_gpio_chip);

PERIPH_MEMORY_DEFINE(imx_gpio, BASE_ADDR(0), (0x4000 * 5));
