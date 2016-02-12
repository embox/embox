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

#define GPIO_PORT_SIZE 0x400
#define GPIO_PORT_BASE 0x40010800

#define GPIO_PORT_NUM 5

typedef volatile unsigned long __gpio_mask_t;

typedef volatile unsigned int stm32_reg_t;

struct gpio {
	stm32_reg_t crl;
	stm32_reg_t crh;
	stm32_reg_t idr;
	stm32_reg_t odr;
	stm32_reg_t bsrr;
	stm32_reg_t brr;
	stm32_reg_t lckr;
} __attribute__ ((packed));

#define GPIO_A ((struct gpio *) (GPIO_PORT_BASE))
#define GPIO_B ((struct gpio *) (GPIO_PORT_BASE + 1 * GPIO_PORT_SIZE))
#define GPIO_C ((struct gpio *) (GPIO_PORT_BASE + 2 * GPIO_PORT_SIZE))
#define GPIO_D ((struct gpio *) (GPIO_PORT_BASE + 3 * GPIO_PORT_SIZE))
#define GPIO_E ((struct gpio *) (GPIO_PORT_BASE + 4 * GPIO_PORT_SIZE))


#endif /* STM32_GPIO_H_ */
