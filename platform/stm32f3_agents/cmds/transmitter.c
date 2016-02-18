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
#include <framework/mod/options.h>

#define AGENT_ID OPTION_GET(NUMBER, agent_id)
#define UART_NUM	3
#define MSG_LEN		16

static Led_TypeDef leds[] = { 0, 2, 4, 6, 7, 5, 3 };

static void leds_off(void) {
	int i;
	for (i = 0; i < sizeof(leds); i++)
		BSP_LED_Off(i);
}

static void init_leds(void) {
	int i;
	for (i = 0; i < sizeof(leds); i++)
		BSP_LED_Init(i);

	leds_off();
}

static int leds_cnt = 0;

static void leds_next(void) {
	if (++leds_cnt == sizeof(leds)) {
		leds_cnt = 0;
		leds_off();
	} else {
		BSP_LED_On(leds[leds_cnt]);
	}
}

static void leds_prev(void) {
	BSP_LED_Off(leds_cnt);
	if (--leds_cnt < 0)
		leds_cnt = 0;
}

static int current_state;

static USART_TypeDef *uart_base[UART_NUM] = {
	(void*) USART1,
	(void*) USART2,
	(void*) USART3
};

static void transmit_delay(void) {
}

static int obtain_data(void) {
	return current_state;
}

static void transmit_data(int data, USART_TypeDef *uart) {
	while ((STM32_USART_FLAGS(uart) & USART_FLAG_TXE) == 0);

	STM32_USART_TXDATA(uart) = (uint8_t) data;
}

static void *transmitter_thread_run(void *arg) {
	int i;
	int data;

	while (1) {
		transmit_delay();
		data = obtain_data();
		for (i = 0; i < UART_NUM; i++)
			transmit_data(data, uart_base[i]);
	}

	return NULL;
}

static void process_message(char *msg) {
	/* TODO */
}

static int message_valid(char *msg) {
	return 1;
}

static void *receiver_thread_run(void *arg) {
	int i;
	char buf[UART_NUM][MSG_LEN];
	int counter[UART_NUM];

	while (1) {
		for (i = 0; i < UART_NUM; i++) {
			if (STM32_USART_FLAGS(uart_base[i]) & USART_FLAG_RXNE) {
				buf[i][counter[i]++] = STM32_USART_RXDATA(uart_base[i]) & 0xFF;
				if (counter[i] == MSG_LEN) {
					/* Message finished */
					if (!message_valid(buf[i])) {
						counter[i] = 0;
						memset(buf[i], 0, sizeof(buf[i]));
						continue;
					} else {
						process_message(buf[i]);
					}
				}
			}
		}
	}

	return NULL;
}

/* Receive char via uart and turn on LED */
void *led_handler(void *arg) {
	while (1) {
		/* Read port */
	}
	return NULL;
}

int main() {
	BSP_PB_Init(0, 0);

	thread_create(0, transmitter_thread_run, NULL);
	receiver_thread_run(NULL);

	return 0;
}
