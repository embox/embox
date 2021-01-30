/**
 * @file
 *
 * @date 30.01.2021
 * @author Vadim Deryabkin
 */

#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F0_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F0_H_

#include <assert.h>
#include <framework/mod/options.h>

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t RES_1;
    volatile uint32_t RTOR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
} usart_struct; // Doc: DocID025023 Rev 4, 640/779.

// Doc: DS9773 Rev 4, 40/93.
#define USART1    ((usart_struct *)      0x40013800)
#define USART2    ((usart_struct *)      0x40004400)
#define USART3    ((usart_struct *)      0x40004800)
#define USART4    ((usart_struct *)      0x40004C00)
#define USART5    ((usart_struct *)      0x40005000)
#define USART6    ((usart_struct *)      0x40011400)

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)

#define USART1_IRQ 	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f0, NUMBER, usart1_irq)
#define USART2_IRQ 	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f0, NUMBER, usart2_irq)
#define USART3_IRQ 	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f0, NUMBER, usart3_4_5_6)
#define USART4_IRQ 	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f0, NUMBER, usart3_4_5_6)
#define USART5_IRQ 	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f0, NUMBER, usart3_4_5_6)
#define USART6_IRQ 	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f0, NUMBER, usart3_4_5_6)

#if MODOPS_USARTX == 1
#define USARTx                           USART1
#define USARTx_IRQn                      USART1_IRQ
#elif MODOPS_USARTX == 2
#define USARTx                           USART2
#define USARTx_IRQn                      USART2_IRQ
#elif MODOPS_USARTX == 3
#define USARTx                           USART3
#define USARTx_IRQn                      USART3_IRQ
#elif MODOPS_USARTX == 4
#define USARTx                           USART4
#define USARTx_IRQn                      USART4_IRQ
#elif MODOPS_USARTX == 5
#define USARTx                           USART5
#define USARTx_IRQn                      USART5_IRQ
#elif MODOPS_USARTX == 6
#define USARTx                           USART6
#define USARTx_IRQn                      USART6_IRQ
#else
#error Unsupported USARTx
#endif

#endif /* SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F0_H_ */
