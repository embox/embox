/**
 * @file
 *
 * @brief GPIO driver for STM32 mcu that will be the base to
 * genereal gpio driver, then all mcu's specific data types and
 * defenitions will go to right place for them.
 *
 * @author Anton Kozlov
 * @date 20.04.2012
 */

#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#define GPIO_PORT_SIZE 0x400
#define GPIO_PORT_BASE 0x40010800

#define GPIO_PORT_NUM 5

typedef volatile u32_t gpio_mask_t;

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

extern void gpio_conf_out(struct gpio *gpio, gpio_mask_t mask);
extern void gpio_out_set(struct gpio *gpio, gpio_mask_t mask);
extern void gpio_conf_in_pull_up(struct gpio *gpio, gpio_mask_t mask);
extern void gpio_conf_in_pull_down(struct gpio *gpio, gpio_mask_t mask);
extern gpio_mask_t gpio_in_get(struct gpio *gpio, gpio_mask_t mask);

#endif /* DRIVERS_GPIO_H_ */
