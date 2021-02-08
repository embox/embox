/*
 * stm32_gpio_common_struct.h
 *
 *  Created on: 07.02.2021
 *      Author: vadim
 */

#ifndef SRC_DRIVERS_GPIO_STM32_STM32_GPIO_COMMON_STRUCT_H_
#define SRC_DRIVERS_GPIO_STM32_STM32_GPIO_COMMON_STRUCT_H_

// Doc for stm32f1: DocID025023 Rev 4, 143/779.
// Doc for stm32l0x0: DocID031151 Rev 1, RM0451, 205/774.
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
} gpio_struct;

#endif /* SRC_DRIVERS_GPIO_STM32_STM32_GPIO_COMMON_STRUCT_H_ */
