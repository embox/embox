/**
 * @file
 * @brief
 *
 * @date 10.01.2017
 * @author Alex Kalmuk
 */

#include <string.h>
#include <assert.h>

#include <kernel/irq.h>

#include "stm32f4_discovery.h"
#include "ir.h"

static irq_return_t exti1_handler(unsigned int irq_nr, void *data) {
	struct ir_receiver *ir_rcv = (struct ir_receiver *) data;

	if (__HAL_GPIO_EXTI_GET_IT(ir_rcv->in_pin) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(ir_rcv->in_pin);
	}

	ir_rcv->pulse_count++;

	return IRQ_HANDLED;
}

static void ir_clk_enable(GPIO_TypeDef *GPIOx) {
	/* FIXME Add ability to use any GPIO */
	assert(GPIOx == GPIOD || GPIOx == GPIOE);
	if (GPIOx == GPIOD) {
		__GPIOD_CLK_ENABLE();
	} else if (GPIOx == GPIOE){
		__GPIOE_CLK_ENABLE();
	}
}

void ir_led_init(struct ir_led *led, int out_pin,
					GPIO_TypeDef *GPIOx) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

	led->GPIOx = GPIOx;
	led->out_pin = out_pin;

	ir_clk_enable(led->GPIOx);

	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStruct.Pin       = led->out_pin;
	HAL_GPIO_Init(led->GPIOx, &GPIO_InitStruct);

	ir_led_enable(led);
}

void ir_receiver_init(struct ir_receiver *rcv, int in_pin,
						GPIO_TypeDef *GPIOx, int irq_nr) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

	rcv->GPIOx = GPIOx;
	rcv->in_pin = in_pin;
	rcv->pulse_count = 0;

	ir_clk_enable(rcv->GPIOx);

	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Pin  = rcv->in_pin;
	HAL_GPIO_Init(rcv->GPIOx, &GPIO_InitStruct);

	irq_attach(irq_nr + 16, exti1_handler, 0, rcv, "IR receiver");
}
