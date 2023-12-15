/**
 * @brief
 *
 * @date 14.12.23
 * @author Aleksey Zhmulin
 */
#include <drivers/common/memory.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/ttys.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>

#define BASE_ADDR OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM   OPTION_GET(NUMBER, irq_num)
#define BAUD_RATE OPTION_GET(NUMBER, baud_rate)

extern const struct uart_ops imx_uart_ops;

static struct uart imx_uart_ttyS0 = {
    .uart_ops = &imx_uart_ops,
    .irq_num = IRQ_NUM,
    .base_addr = BASE_ADDR,
    .params = {.baud_rate = BAUD_RATE,
        .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD
                            | UART_PARAM_FLAGS_USE_IRQ},
};

PERIPH_MEMORY_DEFINE(imx_uart_ttyS0, BASE_ADDR, 0xc0);

STATIC_IRQ_ATTACH(IRQ_NUM, uart_irq_handler, &imx_uart_ttyS0);

TTYS_DEF(ttyS0, &imx_uart_ttyS0);
