/**
 * @file
 *
 * @date 30.01.2021
 * @author Vadim Deryabkin
 */

#ifndef SRC_DRIVERS_GPIO_STM32_STM32_GPIO_CONF_F0_H_
#define SRC_DRIVERS_GPIO_STM32_STM32_GPIO_CONF_F0_H_

#include <hal/reg.h>

#define PINS_NUMBER            16
#define STM32_GPIO_PORTS_COUNT 6

#define REG_RCC        OPTION_GET(NUMBER, reg_rcc_address) // Doc: DS9773 Rev 4, 39/93.
#define REG_RCC_AHBENR (REG_RCC + 0x14) // Doc: DocID025023 Rev 4, 125/779.

#include "stm32_gpio_common_struct.h"

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
#define GPIOA  ((gpio_struct *)0x48000000)
#define EXTI   ((exti_struct *)0x40010400)
#define SYSCFG ((syscfg_struct *)0x40010000)

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
#define L_AF(N, AFx) (AFx << (4 * N))
#define L_AF_MSK(N)  (0b1111 << (4 * N))
#define H_AF(N, AFx) (AFx << (4 * (N - 8)))
#define H_AF_MSK(N)  (0b1111 << (4 * (N - 8)))

#define STM32_GPIO_CHIP_ID OPTION_GET(NUMBER, gpio_chip_id)

#define EXTI0_1_IRQ  OPTION_GET(NUMBER, exti0_1_irq)
#define EXTI2_3_IRQ  OPTION_GET(NUMBER, exti2_3_irq)
#define EXTI4_15_IRQ OPTION_GET(NUMBER, exti4_15_irq)

static void set_gpio_pwr(unsigned int port) {
	REG32_ORIN(REG_RCC_AHBENR,
	    (1 << (port + 17))); // Doc: DocID025023 Rev 4, 111/779.
}

#endif /* SRC_DRIVERS_GPIO_STM32_STM32_GPIO_CONF_F0_H_ */
