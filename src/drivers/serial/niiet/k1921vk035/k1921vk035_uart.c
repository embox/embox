#include <stdint.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/gpio.h>
#include <drivers/ttys.h>

#include <framework/mod/options.h>

#include <embox/unit.h>
#include "kernel/irq.h"
#include "plib035.h"

#include "plib035_uart.h"

#define USE_E_RT_IRQ OPTION_GET(BOOLEAN, use_e_rt_irq)
#define USE_FIFO OPTION_GET(BOOLEAN, use_fifo)

#define UART_GPIO			GPIO_PORT_B
#define UART0_GPIO_TX_mask		(1<<10)
#define UART0_GPIO_RX_mask		(1<<11)
#define UART1_GPIO_TX_mask		(1<<8)
#define UART1_GPIO_RX_mask		(1<<9)

static int k1921vk035_uart_setup(struct uart *dev, const struct uart_params *params) {
    UART_TypeDef* uart = (void* )dev->base_addr;
    UART_Num_TypeDef uart_num;
    gpio_mask_t uart_gpio_mask;
    uint8_t uart_e_rt_irqn;
    if (uart == UART0) {
        uart_num = UART0_Num;
        uart_gpio_mask = UART0_GPIO_TX_mask | UART0_GPIO_RX_mask;
        uart_e_rt_irqn = UART0_E_RT_IRQn;
    } else {
        uart_num = UART1_Num;
        uart_gpio_mask = UART1_GPIO_TX_mask | UART1_GPIO_RX_mask;
        uart_e_rt_irqn = UART1_E_RT_IRQn;
    }

    RCU_UARTClkConfig(uart_num, RCU_PeriphClk_PLLClk, 0, DISABLE);
    RCU_UARTClkCmd(uart_num, ENABLE);
    RCU_UARTRstCmd(uart_num, ENABLE);

    // This also enables gpio port
    gpio_setup_mode(GPIO_PORT_B, uart_gpio_mask, GPIO_MODE_ALT_SET(0));

    UART_Init_TypeDef uart_init_struct;
    UART_StructInit(&uart_init_struct);

    uart_init_struct.BaudRate = params->baud_rate;
    uart_init_struct.DataWidth = UART_DataWidth_8;
#if USE_FIFO
    uart_init_struct.FIFO = ENABLE;
#else
    uart_init_struct.FIFO = DISABLE;
#endif

	switch (UART_PARAM_FLAGS_PARITY_MASK(params->uart_param_flags)) {
		case UART_PARAM_FLAGS_PARITY_ODD:
			uart_init_struct.ParityBit = UART_ParityBit_Odd;
			break;
		case UART_PARAM_FLAGS_PARITY_EVEN:
			uart_init_struct.ParityBit = UART_ParityBit_Even;
			break;
		case UART_PARAM_FLAGS_PARITY_NONE:
		default:
			uart_init_struct.ParityBit = UART_ParityBit_Disable;
			break;
    }

	switch (UART_PARAM_FLAGS_STOPS_MASK(params->uart_param_flags)) {
		case UART_PARAM_FLAGS_2_STOP:
			uart_init_struct.StopBit = UART_StopBit_2;
			break;
		case UART_PARAM_FLAGS_1_STOP:
		default:
			uart_init_struct.StopBit = UART_StopBit_1;
			break;
    }
    uart_init_struct.Rx = ENABLE;
    uart_init_struct.Tx = ENABLE;

    UART_Init(uart, &uart_init_struct);
    UART_Cmd(uart, ENABLE);
    if(params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
#if USE_E_RT_IRQ
        NVIC_EnableIRQ(uart_e_rt_irqn);
        NVIC_SetPriority(uart_e_rt_irqn, 0);
        UART_ITCmd(uart, UART_IMSC_RTIM_Msk | UART_IMSC_RXIM_Msk, ENABLE);
#else
        UART_ITCmd(uart, UART_IMSC_RXIM_Msk, ENABLE);
#endif

#if USE_FIFO
        UART_ITFIFOLevelRxConfig(uart, UART_FIFOLevel_1_8);
#endif
    }

	return 0;
}

static int k1921vk035_uart_putc(struct uart *dev, int ch) {
    UART_TypeDef* uart = (void* )dev->base_addr;
    while(uart->FR_bit.BUSY) {}
    UART_SendData(uart, ch);

	return 0;
}

static int k1921vk035_uart_hasrx(struct uart *dev) {
    UART_TypeDef* uart = (void* )dev->base_addr;

	return !uart->FR_bit.RXFE;
}

static int k1921vk035_uart_getc(struct uart *dev) {
    UART_TypeDef* uart = (void* )dev->base_addr;
#if 0
    uint32_t dr = uart->DR;
    if(dr & 0x0F00) {
        return -1; /* Error */
    } else {
        return (int) UART_RecieveData(uart);
    }
#else
    return (int) UART_RecieveData(uart);
#endif
}

static int k1921vk035_uart_irq_en(struct uart *dev, const struct uart_params *params) {
    UART_TypeDef* uart = (void* )dev->base_addr;
    UART_ITCmd(uart, UART_IMSC_RXIM_Msk, ENABLE);

	return 0;
}

static int k1921vk035_uart_irq_dis(struct uart *dev, const struct uart_params *params) {
    UART_TypeDef* uart = (void* )dev->base_addr;
    UART_ITCmd(uart, UART_IMSC_RXIM_Msk, DISABLE);

	return 0;
}

const struct uart_ops k1921vk035_uart_ops = {
		.uart_setup = k1921vk035_uart_setup,
		.uart_putc  = k1921vk035_uart_putc,
		.uart_hasrx = k1921vk035_uart_hasrx,
		.uart_getc  = k1921vk035_uart_getc,
		.uart_irq_en = k1921vk035_uart_irq_en,
		.uart_irq_dis = k1921vk035_uart_irq_dis,
};
