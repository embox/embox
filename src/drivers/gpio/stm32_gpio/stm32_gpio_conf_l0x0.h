/**
 * @file
 *
 * @date 07.02.2021
 * @author Vadim Deryabkin
 */

#ifndef SRC_DRIVERS_GPIO_STM32_STM32_GPIO_CONF_L0X0_H_
#define SRC_DRIVERS_GPIO_STM32_STM32_GPIO_CONF_L0X0_H_

#include <hal/reg.h>

#define PINS_NUMBER            16
#define STM32_GPIO_PORTS_COUNT 6

#define REG_RCC OPTION_GET(NUMBER, reg_rcc_address)

// Doc: DocID031151 Rev 1, RM0451, 169/774.
#define REG_RCC_IOPENR (REG_RCC + 0x2C)

#include "stm32_gpio_common_struct.h"

typedef struct {
	volatile uint32_t IMR;   // 0x00
	volatile uint32_t EMR;   // 0x04
	volatile uint32_t RTSR;  // 0x08
	volatile uint32_t FTSR;  // 0x0C
	volatile uint32_t SWIER; // 0x10
	volatile uint32_t PR;    // 0x14
} exti_struct;               // Doc: DocID031151 Rev 1, RM0451, 245/774.

typedef struct {
	volatile uint32_t CFGR1;     // 0x00
	volatile uint32_t CFGR2;     // 0x04
	volatile uint32_t EXTICR[4]; // 0x08
	volatile uint32_t RES[2];    // 0x18
	volatile uint32_t CFGR3;     // 0x20
} syscfg_struct;                 // Doc: DocID031151 Rev 1, RM0451, 213/774.

// Doc: DocID031151 Rev 1, RM0451, 39/774.
#define GPIOA  ((gpio_struct *)0X50000000)
#define EXTI   ((exti_struct *)0X40010400)
#define SYSCFG ((syscfg_struct *)0X40010000)

// Doc: DocID031151 Rev 1, RM0451, 198/774.
#define M_PP(N)  (0b01 << 2 * N)
#define M_ALT(N) (0b10 << 2 * N)
#define M_A(N)   (0b11 << 2 * N)
#define M_MSK(N) (0b11 << 2 * N)

// Doc: DocID031151 Rev 1, RM0451, 199/774.
#define S_HS(N)  (0b11 << 2 * N)
#define S_MSK(N) (0b11 << 2 * N)

// Doc: DocID031151 Rev 1, RM0451, 199/774.
#define P_PU(N)  (0b01 << 2 * N)
#define P_PD(N)  (0b10 << 2 * N)
#define P_MSK(N) (0b11 << 2 * N)

// Doc: DocID031151 Rev 1, RM0451, 202/774.
#define L_AF(N, AFx) (AFx << (4 * N))
#define L_AF_MSK(N)  (0b1111 << (4 * N))
#define H_AF(N, AFx) (AFx << (4 * (N - 8)))
#define H_AF_MSK(N)  (0b1111 << (4 * (N - 8)))

#define STM32_GPIO_CHIP_ID OPTION_GET(NUMBER, gpio_chip_id)

#define EXTI0_1_IRQ  OPTION_GET(NUMBER, exti0_1_irq)
#define EXTI2_3_IRQ  OPTION_GET(NUMBER, exti2_3_irq)
#define EXTI4_15_IRQ OPTION_GET(NUMBER, exti4_15_irq)

static void set_gpio_pwr(unsigned int port) {
	REG32_ORIN(REG_RCC_IOPENR,
	    (1 << port)); // Doc: DocID031151 Rev 1, RM0451, 169/774.
}

#endif /* SRC_DRIVERS_GPIO_STM32_STM32_GPIO_CONF_L0X0_H_ */
