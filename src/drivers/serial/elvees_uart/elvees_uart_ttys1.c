/**
 * @file
 *
 * @date Feb 11, 2023
 * @author Anton Bondarev
 */

 #include <util/macro.h>

 #include <kernel/irq.h>
 #include <drivers/common/memory.h>
 
 #include <drivers/serial/uart_dev.h>
 #include <drivers/ttys.h>
 
 #include <framework/mod/options.h>
 
 #define UART_BASE      OPTION_GET(NUMBER, base_addr)
 #define IRQ_NUM        OPTION_GET(NUMBER, irq_num)
 #define BAUD_RATE      OPTION_GET(NUMBER,baud_rate)
 
 #define TTY_NAME    ttyS1

 #define TTY_STRUCT_NAME(x)    uart_ ## x

 extern const struct uart_ops elvees_uart_uart_ops;
 
 static struct uart TTY_STRUCT_NAME(TTY_NAME) = {
		 .dev_name = MACRO_STRING(TTY_NAME),
		 .uart_ops = &elvees_uart_uart_ops,
		 .irq_num = IRQ_NUM,
		 .base_addr = UART_BASE,
		 .params =  {
			 .baud_rate = BAUD_RATE,
			 .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD |
			 						UART_PARAM_FLAGS_USE_IRQ,
		 },
 };
 
 TTYS_DEF(TTY_NAME, &TTY_STRUCT_NAME(TTY_NAME));
 
 PERIPH_MEMORY_DEFINE(elvees_uart, UART_BASE, 0x1000);
 
 extern irq_return_t uart_irq_handler(unsigned int irq_nr, void *data);
 STATIC_IRQ_ATTACH(IRQ_NUM, uart_irq_handler, &TTY_STRUCT_NAME(TTY_NAME));