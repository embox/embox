#include "types.h"
#include "irq.h"
#include "test.h"
#include "conio.h"
#include "leon.h"
#include "common.h"
#include "timers.h"
#include "uart.h"
#include "sys.h"

void test_abort_handler() {
	char ch = uart_getc();
	if (ch == 'Q' || ch == 'q') {
		sys_interrupt();
	}
}

void test_abort_enable() {
	uart_set_irq_handler(test_abort_handler);
}
void test_abort_disable() {
	uart_remove_irq_handler(test_abort_handler);
}
