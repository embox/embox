/**
 * @file stm32_gpio_f0.c
 * @brief
 * @author Vadim Deryabkin
 * @version
 * @date 29.01.2021
 */

#include <assert.h>
#include <string.h>

#include <hal/reg.h>

#include <embox/unit.h>
#include <util/array.h>

#include <drivers/gpio/gpio_driver.h>

#define PINS_NUMBER 16
#define STM32_GPIO_PORTS_COUNT 6

#define REG_RCC        0x40021000       // Doc: DS9773 Rev 4, 39/93.
#define REG_RCC_AHBENR (REG_RCC + 0x14) // Doc: DocID025023 Rev 4, 125/779.

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFRL;
    volatile uint32_t AFRH;
    volatile uint32_t BRR;
} gpio_struct; // Doc: DocID025023 Rev 4, 143/779.

typedef struct {
    volatile uint32_t IMR;
    volatile uint32_t EMR;
    volatile uint32_t RTSR;
    volatile uint32_t FTSR;
    volatile uint32_t SWIER;
    volatile uint32_t PR;
} exti_struct; // Doc: DocID025023 Rev 4, 176/779.

typedef struct {
    volatile uint32_t CFGR1;
    volatile uint32_t RES_1;
    volatile uint32_t EXTICR[4];
    volatile uint32_t CFGR2;
} syscfg_struct; // Doc: DocID025023 Rev 4, 147/779.

// Doc: DS9773 Rev 4, 39/93.
#define GPIOA     ((gpio_struct *)         0x48000000)
#define EXTI      ((exti_struct *)         0x40010400)
#define SYSCFG    ((syscfg_struct *)       0x40010000)

// Doc: DocID025023 Rev 4, 136/779.
#define M_PP(N)  (0b01 << 2 * N)
#define M_ALT(N) (0b10 << 2 * N)
#define M_A(N)   (0b11 << 2 * N)
#define M_MSK(N) (0b11 << 2 * N)

// Doc: DocID025023 Rev 4, 137/779.
#define P_PU(N)  (0b01 << 2 * N)
#define P_PD(N)  (0b10 << 2 * N)
#define P_MSK(N) (0b11 << 2 * N)

// Doc: DocID025023 Rev 4, 137/779.
#define S_MS(N)  (0b01 << 2 * N)
#define S_HS(N)  (0b11 << 2 * N)
#define S_MSK(N) (0b11 << 2 * N)

// Doc: DocID025023 Rev 4, 140/779.
#define L_AF(N,AFx) (AFx << (4 * N))
#define L_AF_MSK(N) (0b1111 << (4 * N))
#define H_AF(N,AFx) (AFx << (4 * (N - 8)))
#define H_AF_MSK(N) (0b1111 << (4 * (N - 8)))

#define STM32_GPIO_CHIP_ID	OPTION_GET(NUMBER,gpio_chip_id)

#define EXTI0_1_IRQ			OPTION_GET(NUMBER, exti0_1_irq)
#define EXTI2_3_IRQ			OPTION_GET(NUMBER, exti2_3_irq)
#define EXTI4_15_IRQ		OPTION_GET(NUMBER, exti4_15_irq)

static void set_gpio_pwr (unsigned char port) {
	REG32_ORIN(REG_RCC_AHBENR, (1 << (port + 17))); // Doc: DocID025023 Rev 4, 111/779.
}

static gpio_struct *get_gpio (unsigned char port) {
	uint8_t *p = (uint8_t *)GPIOA;
	p += 1024 * port;
	return (gpio_struct *)p;
}

static void set_gpio_mode_output (gpio_struct* GPIO, gpio_mask_t pins) {
	uint32_t buf_mode = 0;
	uint32_t msk_mode = 0;
	for (int i = 0; i < PINS_NUMBER; i++) {
		if (pins & (1 << i)) {
			buf_mode |= M_PP(i);
			msk_mode |= M_MSK(i);
		}
	}

	GPIO->MODER &= ~msk_mode;
	GPIO->MODER |= buf_mode;
}

static void set_gpio_mode_input (gpio_struct* GPIO, gpio_mask_t pins) {
	uint32_t msk_mode = 0;
	for (int i = 0; i < PINS_NUMBER; i++) {
		if (pins & (1 << i)) {
			msk_mode |= M_MSK(i);
		}
	}

	GPIO->MODER &= ~msk_mode;
}

static void set_gpio_mode_pull_up (gpio_struct* GPIO, gpio_mask_t pins) {
	uint32_t buf_mode = 0;
	uint32_t msk_mode = 0;
	for (int i = 0; i < PINS_NUMBER; i++) {
		if (pins & (1 << i)) {
			buf_mode |= P_PU(i);
			msk_mode |= P_MSK(i);
		}
	}

	GPIO->PUPDR &= ~msk_mode;
	GPIO->PUPDR |= buf_mode;
}

static void set_gpio_mode_pull_down (gpio_struct* GPIO, gpio_mask_t pins) {
	uint32_t buf_mode = 0;
	uint32_t msk_mode = 0;
	for (int i = 0; i < PINS_NUMBER; i++) {
		if (pins & (1 << i)) {
			buf_mode |= P_PD(i);
			msk_mode |= P_MSK(i);
		}
	}

	GPIO->PUPDR &= ~msk_mode;
	GPIO->PUPDR |= buf_mode;
}

static void set_gpio_high_spped (gpio_struct* GPIO, gpio_mask_t pins) {
	uint32_t buf_mode = 0;
	uint32_t msk_mode = 0;
	for (int i = 0; i < PINS_NUMBER; i++) {
		if (pins & (1 << i)) {
			buf_mode |= S_HS(i);
			msk_mode |= S_MSK(i);
		}
	}

	GPIO->OSPEEDR &= ~msk_mode;
	GPIO->OSPEEDR |= buf_mode;
}

static void set_gpio_mode_alt_func (gpio_struct* GPIO, gpio_mask_t pins,
									uint32_t func) {
	uint32_t buf_mode = 0;
	uint32_t msk_mode = 0;
	for (int i = 0; i < PINS_NUMBER / 2; i++) {
		if (pins & (1 << i)) {
			buf_mode |= L_AF(i, func);
			msk_mode |= L_AF_MSK(i);
		}
	}

	GPIO->AFRL &= msk_mode;
	GPIO->AFRL |= buf_mode;

	buf_mode = 0;
	msk_mode = 0;
	for (int i = PINS_NUMBER / 2; i < PINS_NUMBER; i++) {
		if (pins & (1 << i)) {
			buf_mode |= H_AF(i, func);
			msk_mode |= H_AF_MSK(i);
		}
	}

	GPIO->AFRH &= msk_mode;
	GPIO->AFRH |= buf_mode;
}

static void set_gpio_mode_open_drain (gpio_struct* GPIO, gpio_mask_t pins) {
	GPIO->OTYPER |= pins;
}

static void set_gpio_mode_push_pull (gpio_struct* GPIO, gpio_mask_t pins) {
	GPIO->OTYPER &= ~pins;
}

static void set_gpio_it_rising (gpio_mask_t pins) {
	EXTI->RTSR |= pins;
}

static void reset_gpio_it_rising (gpio_mask_t pins) {
	EXTI->RTSR &= ~pins;
}

static void set_gpio_it_falling (gpio_mask_t pins) {
	EXTI->FTSR |= pins;
}

static void reset_gpio_it_falling (gpio_mask_t pins) {
	EXTI->FTSR &= ~pins;
}

static void set_exti_interrupt (unsigned char port, gpio_mask_t pins) {
	for (int i = 0; i < PINS_NUMBER; i++) {
		if (pins & (1 << i)) {
			SYSCFG->EXTICR[i / 4] &= ~(0xF << 4 * (i % 4)); // DocID025023 Rev 4, 147/779.
			SYSCFG->EXTICR[i / 4] |= port << 4 * (i % 4);
		}
	}

	EXTI->PR = pins;
	EXTI->EMR |= pins;
	EXTI->IMR |= pins;
}

static void reset_exti_interrupt (unsigned char port, gpio_mask_t pins) {
	EXTI->EMR &= ~pins;
	EXTI->IMR &= ~pins;
	EXTI->PR = pins;
}

static int stm32_gpio_setup_mode(unsigned char port, gpio_mask_t pins, int mode) {
	assert(!((port == GPIO_PORT_E) || (port > GPIO_PORT_F)));

	set_gpio_pwr(port);

	gpio_struct* GPIO = get_gpio(port);

	set_gpio_high_spped(GPIO, pins);

	if (mode & GPIO_MODE_OUTPUT) {
		set_gpio_mode_output(GPIO, pins);

		if (mode & GPIO_MODE_OUT_OPEN_DRAIN)	set_gpio_mode_open_drain(GPIO, pins);
		if (mode & GPIO_MODE_OUT_PUSH_PULL)		set_gpio_mode_push_pull(GPIO, pins);
		reset_exti_interrupt(port, pins);
	} else if (mode & GPIO_MODE_INPUT) {
		set_gpio_mode_input(GPIO, pins);

		if (mode & GPIO_MODE_IN_PULL_UP) 		set_gpio_mode_pull_up(GPIO, pins);
		if (mode & GPIO_MODE_IN_PULL_DOWN)		set_gpio_mode_pull_down(GPIO, pins);
		reset_exti_interrupt(port, pins);
	} else if (mode & GPIO_MODE_OUT_ALTERNATE) {
		uint32_t func = GPIO_GET_ALTERNATE(mode);
		set_gpio_mode_alt_func(GPIO, pins, func);

		if (mode & GPIO_MODE_IN_PULL_UP) 		set_gpio_mode_pull_up(GPIO, pins);
		if (mode & GPIO_MODE_IN_PULL_DOWN)		set_gpio_mode_pull_down(GPIO, pins);
		if (mode & GPIO_MODE_OUT_PUSH_PULL)		set_gpio_mode_push_pull(GPIO, pins);
		if (mode & GPIO_MODE_OUT_OPEN_DRAIN)	set_gpio_mode_open_drain(GPIO, pins);

		reset_exti_interrupt(port, pins);
	} else if (mode & GPIO_MODE_INT_MODE_RISING_FALLING) {
		if ((mode & GPIO_MODE_INT_MODE_RISING_FALLING) ==
				GPIO_MODE_INT_MODE_RISING_FALLING) {
			set_gpio_it_rising(pins);
			set_gpio_it_falling(pins);
		} else if (mode & GPIO_MODE_INT_MODE_RISING) {
			set_gpio_it_rising(pins);
			reset_gpio_it_falling(pins);
		} else if (mode & GPIO_MODE_INT_MODE_FALLING) {
			reset_gpio_it_rising(pins);
			set_gpio_it_falling(pins);
		}

		set_exti_interrupt(port, pins);
	}

	return 0;
}

static void stm32_gpio_set(unsigned char port, gpio_mask_t pins, char level) {
	assert(!((port == GPIO_PORT_E) || (port > GPIO_PORT_F)));

	gpio_struct* GPIO = get_gpio(port);

	if (level) {
		GPIO->BSRR = pins;
	} else {
		GPIO->BRR = pins;
	}
}

static gpio_mask_t stm32_gpio_get(unsigned char port, gpio_mask_t pins) {
	assert(!((port == GPIO_PORT_E) || (port > GPIO_PORT_F)));

	gpio_struct* GPIO = get_gpio(port);

	return GPIO->IDR & pins;
}


static struct gpio_chip stm32_gpio_chip = {
	.setup_mode = stm32_gpio_setup_mode,
	.get = stm32_gpio_get,
	.set = stm32_gpio_set,
	.nports = STM32_GPIO_PORTS_COUNT
};

irq_return_t stm32_gpio_irq_handler(unsigned int irq_nr, void *data) {
	uint16_t pending = EXTI->PR;
	EXTI->PR = 0xFFFF;

	/* Notify all GPIO ports about interrupt */
	for (int i = 0; i < STM32_GPIO_PORTS_COUNT; i++) {
		gpio_handle_irq(&stm32_gpio_chip, i, pending);
	}

	return IRQ_HANDLED;
}

static const unsigned char exti_irqs[] = {
	EXTI0_1_IRQ, EXTI2_3_IRQ, EXTI4_15_IRQ
};

static int stm32_gpio_init(void) {
	int res, i;

	for (i = 0; i < ARRAY_SIZE(exti_irqs); i++) {
		res = irq_attach(exti_irqs[i], stm32_gpio_irq_handler, 0, NULL,
			"STM32 EXTI irq handler");
		if (res < 0) {
			return -1;
		}
	}

	return gpio_register_chip(&stm32_gpio_chip, STM32_GPIO_CHIP_ID);
}

EMBOX_UNIT_INIT(stm32_gpio_init);

STATIC_IRQ_ATTACH(EXTI0_1_IRQ, stm32_gpio_irq_handler, NULL);
STATIC_IRQ_ATTACH(EXTI2_3_IRQ, stm32_gpio_irq_handler, NULL);
STATIC_IRQ_ATTACH(EXTI4_15_IRQ, stm32_gpio_irq_handler, NULL);
