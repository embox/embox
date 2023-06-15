/**
 * @file
 *
 * @brief GPIO driver for STM32 mcu
 *
 * @author Anton Kozlov
 * @date 20.04.2012
 */

#ifndef STM32_GPIO_H_
#define STM32_GPIO_H_

#include <stdint.h>

#define GPIO_PORT_SIZE 0x400
#define GPIO_PORT_BASE 0x40010800

#define GPIO_PORT_NUM 5

typedef volatile uint32_t stm32_reg_t;

struct stm32_gpio {
	stm32_reg_t crl;
	stm32_reg_t crh;
	stm32_reg_t idr;
	stm32_reg_t odr;
	stm32_reg_t bsrr;
	stm32_reg_t brr;
	stm32_reg_t lckr;
} __attribute__ ((packed));

#define STM32_GPIO(i) \
	((struct stm32_gpio *) (GPIO_PORT_BASE + (i) * GPIO_PORT_SIZE))

#endif /* STM32_GPIO_H_ */
