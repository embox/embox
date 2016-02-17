/**
 * @file
 * @brief Transmit info to port
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-30
 */

#include <agents/communication.h>
#include <agents/communication.h>
#include <kernel/task.h>
#include <kernel/time/ktime.h>
#include <kernel/thread.h>

#include <drivers/serial/stm_usart.h>
#include <drivers/serial/uart_device.h>
#include <framework/mod/options.h>

#define AGENT_ID OPTION_GET(NUMBER, agent_id)
#define UART_NUM 3

static int current_state;

static void *uart_base[UART_NUM] = {
	(void*) USART1,
	(void*) USART2,
	(void*) USART3
};

static void transmit_delay(void) {
}

static int obtain_data(void) {
	return current_state;
}

static void transmit_data(int data, void *uart) {
	while ((STM32_USART_FLAGS(uart) & USART_FLAG_TXE) == 0);

	STM32_USART_TXDATA(uart) = (uint8_t) ch;
}

void transmitter_thread_run(void *arg) {
	int i;
	int data;

	while (1) {
		transmit_delay();
		data = obtain_data();
		for (i = 0; i < UART_NUM; i++)
			transmit_data(data, uart_data[i]);
	}
}

/* Receive char via uart and turn on LED */
void *led_handler(void *arg) {
	uint8_t ch = 0;
	while (1) {
		/* Read port */
	}
	return NULL;
}

int main() {
	uint8_t r = 0;
	diag_init();
	BSP_PB_Init(0, 0);
	new_task(0, led_handler, NULL);

	while (true) {
		int state = BSP_PB_GetState(0);
		if (state) {
			r++;
			r %= 8;
		}
		ksleep(100);
		diag_putc(r);
		ksleep(100);
	}

	return 0;
}
