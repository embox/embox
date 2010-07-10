/**
 * @file at91_uart.c
 * @brief TODO
 *
 * @date 7.06.2010
 * @author Anton Kozlov
 */

#include <types.h>

#include <kernel/diag.h>
#include <kernel/irq.h>


int uart_init(void) {
    diag_init();
    return 0;
}

void uart_putc(char ch) {
    diag_putc(ch);
}

char uart_getc(void) {
    return diag_getc();
}

int uart_set_irq_handler(irq_handler_t pfunc) {
    return 0;
}

int uart_remove_irq_handler(void) {
    return 0;
}
