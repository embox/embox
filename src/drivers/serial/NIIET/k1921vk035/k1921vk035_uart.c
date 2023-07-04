#include <stdint.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/gpio/gpio.h>
#include <drivers/ttys.h>

#include <framework/mod/options.h>

#include <embox/unit.h>
#include "kernel/irq.h"

EMBOX_UNIT_INIT(uart_init);

#include "plib035_uart.h"

#define BAUD_RATE OPTION_GET(NUMBER, baud_rate)
#define USE_UART0_AS_DIAG OPTION_GET(BOOLEAN, use_uart0_as_diag)

#define UART_GPIO			GPIO_PORT_B
#define UART0_GPIO_TX_mask		(1<<10)
#define UART0_GPIO_RX_mask		(1<<11)
#define UART1_GPIO_TX_mask		(1<<8)
#define UART1_GPIO_RX_mask		(1<<9)

static int k1921vk035_uart_setup(struct uart *dev, const struct uart_params *params) {
    UART_TypeDef* uart = (void* )dev->base_addr;
    UART_Num_TypeDef uart_num;
    gpio_mask_t uart_gpio_mask;
    if(uart == UART0) {
        uart_num = UART0_Num;
        uart_gpio_mask = UART0_GPIO_TX_mask | UART0_GPIO_RX_mask;
    }
    else {
        uart_num = UART1_Num;
        uart_gpio_mask = UART1_GPIO_TX_mask | UART1_GPIO_RX_mask;
    }

    RCU_UARTClkConfig(uart_num, RCU_PeriphClk_PLLClk, 0, DISABLE);
    RCU_UARTClkCmd(uart_num, ENABLE);
    RCU_UARTRstCmd(uart_num, ENABLE);

    // This also enables gpio port
    gpio_setup_mode(GPIO_PORT_B, uart_gpio_mask, GPIO_MODE_OUT_ALTERNATE);

    UART_Init_TypeDef uart_init_struct;
    UART_StructInit(&uart_init_struct);

    uart_init_struct.BaudRate = params->baud_rate;
    // TODO: Read DataWidth from uart_params?
    uart_init_struct.DataWidth = UART_DataWidth_8;
    // TODO: Should we enable FIFO?
    uart_init_struct.FIFO = DISABLE;
    // TODO: Read ParityBit from uart_params?
    uart_init_struct.ParityBit = UART_ParityBit_Disable;
    uart_init_struct.StopBit = 
        (params->uart_param_flags & UART_PARAM_FLAGS_2_STOP) ? UART_StopBit_2 : UART_StopBit_1;
    uart_init_struct.Rx = ENABLE;
    uart_init_struct.Tx = ENABLE;

    UART_Init(uart, &uart_init_struct);
    UART_Cmd(uart, ENABLE);
    if(params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
        UART_ITCmd(uart, UART_IMSC_RXIM_Msk, ENABLE);
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

	return (int) UART_RecieveData(uart);
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

static const struct uart_ops k1921vk035_uart_ops = {
		.uart_setup = k1921vk035_uart_setup,
		.uart_putc  = k1921vk035_uart_putc,
		.uart_hasrx = k1921vk035_uart_hasrx,
		.uart_getc  = k1921vk035_uart_getc,
		.uart_irq_en = k1921vk035_uart_irq_en,
		.uart_irq_dis = k1921vk035_uart_irq_dis,
};

#if USE_UART0_AS_DIAG
static struct uart uart0 = {
		.uart_ops = &k1921vk035_uart_ops,
		// .irq_num = UART0_RX_IRQn, used for diag device, does not use interrupts
		.base_addr = (uint32_t)UART0,
        .params = {
            .baud_rate = BAUD_RATE,
            .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_DEV_TYPE_UART,
        },
};

DIAG_SERIAL_DEF(&uart0, &uart0.params);
#else
// User can provide callback
// extern irq_return_t uart0_handler(unsigned int irq_nr, void *data);
__weak irq_return_t uart0_handler(unsigned int irq_nr, void *data) {
    return IRQ_HANDLED;
}

static struct uart uart0 = {
		.uart_ops = &k1921vk035_uart_ops,
		.irq_num = UART0_RX_IRQn, // TODO: Which irq should we use?
		.base_addr = (uint32_t)UART0,
        .irq_handler = uart0_handler,
        .params = {
            .baud_rate = BAUD_RATE,
            .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_DEV_TYPE_UART | UART_PARAM_FLAGS_USE_IRQ, 
        },
};

// NOTE: UART0_RX_IRQn is 26
STATIC_IRQ_ATTACH(26, uart0_handler, &uart0);

#endif // USE_UART0_AS_DIAG

// User can provide callback
// extern irq_return_t uart1_handler(unsigned int irq_nr, void *data);
__weak irq_return_t uart1_handler(unsigned int irq_nr, void *data) {
    return IRQ_HANDLED;
}

static struct uart uart1 = {
		.uart_ops = &k1921vk035_uart_ops,
		.irq_num = UART1_RX_IRQn, // TODO: Which irq should we use?
		.base_addr = (uint32_t)UART1,
        .irq_handler = uart1_handler,
        .params = {
            .baud_rate = BAUD_RATE,
            .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_DEV_TYPE_UART | UART_PARAM_FLAGS_USE_IRQ, 
        },
};

// NOTE: UART1_RX_IRQn is 30
STATIC_IRQ_ATTACH(30, uart1_handler, &uart1);

static int uart_init(void) {
#if USE_UART0_AS_DIAG
    // In this case we only open uart1 
    int retval = uart_register(&uart1, &uart1.params) || uart_open(&uart1);
#else
    // In this case we open both uarts
    int retval = uart_register(&uart0, &uart0.params) || uart_open(&uart0) ||
                 uart_register(&uart1, &uart1.params) || uart_open(&uart1);
#endif // USE_UART0_AS_DIAG
	return retval; 
}



