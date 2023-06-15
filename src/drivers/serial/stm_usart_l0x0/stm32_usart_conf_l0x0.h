/**
 * @file
 *
 * @date 30.01.2021
 * @author Vadim Deryabkin
 */

#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_L0X0_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_L0X0_H_

#include <assert.h>
#include <framework/mod/options.h>

typedef struct {
    volatile uint32_t CR1;  // 0x00
    volatile uint32_t CR2;  // 0x04
    volatile uint32_t CR3;  // 0x08
    volatile uint32_t BRR;  // 0x0C
    volatile uint32_t GTPR; // 0x10
    volatile uint32_t RTOR; // 0x14
    volatile uint32_t RQR;  // 0x18
    volatile uint32_t ISR;  // 0x1C
    volatile uint32_t ICR;  // 0x20
    volatile uint32_t RDR;  // 0x24
    volatile uint32_t TDR;  // 0x28
} usart_struct; // Doc: DocID031151 Rev 1, RM0451, 631/774.

typedef struct {
    volatile uint32_t MODER;   // 0x00
    volatile uint32_t OTYPER;  // 0x04
    volatile uint32_t OSPEEDR; // 0x08
    volatile uint32_t PUPDR;   // 0x0C
    volatile uint32_t IDR;     // 0x10
    volatile uint32_t ODR;     // 0x14
    volatile uint32_t BSRR;    // 0x18
    volatile uint32_t LCKR;    // 0x1C
    volatile uint32_t AFRL;    // 0x20
    volatile uint32_t AFRH;    // 0x24
    volatile uint32_t BRR;     // 0x28
} gpio_struct; // Doc: DocID031151 Rev 1, RM0451, 205/774.

// Doc: DocID031151 Rev 1, RM0451, 198/774.
#define M_ALT(N) (0b10 << 2 * N)
#define M_MSK(N) (0b11 << 2 * N)

// Doc: DocID031151 Rev 1, RM0451, 202/774.
#define L_AF(N,AFx) (AFx << (4 * N))
#define L_AF_MSK(N) (0b1111 << (4 * N))
#define H_AF(N,AFx) (AFx << (4 * (N - 8)))
#define H_AF_MSK(N) (0b1111 << (4 * (N - 8)))

// Doc: DocID031151 Rev 1, RM0451, 199/774.
#define S_HS(N)  (0b11 << 2 * N)
#define S_MSK(N) (0b11 << 2 * N)

// Doc: DocID031151 Rev 1, RM0451, 40/774.
#define USART2    ((usart_struct *)      0X40004400)

// Doc: DocID031151 Rev 1, RM0451, 39/774.
#define GPIOA     ((gpio_struct *)       0X50000000)

#define PINS_NUMBER 16

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)

#define USART2_IRQ 	OPTION_MODULE_GET(embox__driver__serial__stm_usart_l0x0, NUMBER, usart2_irq)

#if MODOPS_USARTX == 2
#define USARTx                           USART2
#define USARTx_IRQn                      USART2_IRQ
#else
#error Unsupported USARTx
#endif

#endif /* SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_L0X0_H_ */
