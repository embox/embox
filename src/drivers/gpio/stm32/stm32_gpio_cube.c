/**
 * @file stm32_gpio_cube.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-12
 */

#include <assert.h>
#include <string.h>

#include <embox/unit.h>
#include <util/bit.h>

#include <drivers/gpio/stm32.h>
#include <drivers/gpio/gpio_driver.h>

#define STM32_GPIO_CHIP_ID OPTION_GET(NUMBER,gpio_chip_id)

#define EXTI_LINES_CNT 4

#define EXTI0_IRQ (EXTI0_IRQn + 16)

EMBOX_UNIT_INIT(stm32_gpio_init);

static struct gpio_chip stm32_gpio_chip;

static int stm32_gpio_setup_mode(unsigned char port, gpio_mask_t pins,
		int mode) {
	GPIO_InitTypeDef GPIO_InitStruct;

	assert(port < STM32_GPIO_PORTS_COUNT);

	stm32_gpio_clk_enable(stm32_gpio_ports[port]);

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));
	GPIO_InitStruct.Pin = pins;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	if (mode & GPIO_MODE_OUTPUT) {
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

		if (mode & GPIO_MODE_OUT_OPEN_DRAIN) {
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		}
		if (mode & GPIO_MODE_OUT_PUSH_PULL) {
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		}
	} else if (mode & GPIO_MODE_INPUT) {
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

		if (mode & GPIO_MODE_IN_PULL_UP) {
			GPIO_InitStruct.Pull = GPIO_PULLUP;
		}
		if (mode & GPIO_MODE_IN_PULL_DOWN) {
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		}
	} else if (mode & GPIO_MODE_OUT_ALTERNATE) {
		GPIO_InitStruct.Alternate = GPIO_GET_ALTERNATE(mode);

		if (mode & GPIO_MODE_OUT_OPEN_DRAIN) {
			GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		}
		if (mode & GPIO_MODE_OUT_PUSH_PULL) {
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		}

		if (mode & GPIO_MODE_IN_PULL_UP) {
			GPIO_InitStruct.Pull = GPIO_PULLUP;
		}
		if (mode & GPIO_MODE_IN_PULL_DOWN) {
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		}
	} else if (mode & GPIO_MODE_INT_MODE_RISING) {
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	} else if (mode & GPIO_MODE_INT_MODE_FALLING) {
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	}

	HAL_GPIO_Init(stm32_gpio_ports[port], &GPIO_InitStruct);

	return 0;
}

static void stm32_gpio_set(unsigned char port, gpio_mask_t pins, char level) {
	assert(port < STM32_GPIO_PORTS_COUNT);
	HAL_GPIO_WritePin(stm32_gpio_ports[port], pins,
		level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static gpio_mask_t stm32_gpio_get(unsigned char port, gpio_mask_t pins) {
	gpio_mask_t res = 0;
	int bit;

	assert(port < STM32_GPIO_PORTS_COUNT);

	bit_foreach(bit, pins) {
		res |= HAL_GPIO_ReadPin(stm32_gpio_ports[port], 1 << bit) << bit;
	}

	return res;
}

irq_return_t stm32_gpio_irq_handler(unsigned int irq_nr, void *data) {
	int i;
	unsigned int pin = 1 << (irq_nr - EXTI0_IRQ);

	if (__HAL_GPIO_EXTI_GET_IT(pin) == RESET) {
		return IRQ_HANDLED;
	}
	 __HAL_GPIO_EXTI_CLEAR_IT(pin);

	/* Notify all GPIO ports about interrupt */
	for (i = 0; i < STM32_GPIO_PORTS_COUNT; i++) {
		gpio_handle_irq(&stm32_gpio_chip, irq_nr, i, pin);
	}
	return IRQ_HANDLED;
}

static struct gpio_chip stm32_gpio_chip = {
	.setup_mode = stm32_gpio_setup_mode,
	.get = stm32_gpio_get,
	.set = stm32_gpio_set,
	.nports = STM32_GPIO_PORTS_COUNT
};

static int stm32_gpio_init(void) {
	int res, i;

	for (i = 0; i < EXTI_LINES_CNT; i++) {
		res = irq_attach(EXTI0_IRQ + i, stm32_gpio_irq_handler, 0, NULL,
			"STM32 EXTI irq handler");
		if (res < 0) {
			return -1;
		}
	}

	return gpio_register_chip(&stm32_gpio_chip, STM32_GPIO_CHIP_ID);
}
