/**
 * @file
 * @brief
 *
 * @date 11.01.2017
 * @author Alex Kalmuk
 */

#ifndef IR_H_
#define IR_H_

#include "stm32f4_discovery.h"

struct ir_led {
	int out_pin;
	GPIO_TypeDef  *GPIOx;
};

struct ir_receiver {
	int pulse_count; /* Number of received pulses */
	int in_pin;
	GPIO_TypeDef  *GPIOx;
};

extern void ir_led_init(struct ir_led *led, int out_pin,
						GPIO_TypeDef *GPIOx);
extern void ir_receiver_init(struct ir_receiver *rcv, int in_pin,
						GPIO_TypeDef *GPIOx, int irq_nr);

static inline int ir_receiver_pulse_count(struct ir_receiver *rcv) {
	return rcv->pulse_count;
}

static inline void ir_led_enable(struct ir_led *led) {
	HAL_GPIO_WritePin(led->GPIOx, led->out_pin, GPIO_PIN_SET);
}

static inline void ir_led_disable(struct ir_led *led) {
	HAL_GPIO_WritePin(led->GPIOx, led->out_pin, GPIO_PIN_RESET);
}

#endif /* IR_H_ */
