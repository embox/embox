/**
 * @file
 * @brief Transmit info to port
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-30
 */

#include <unistd.h>

#include <kernel/task.h>
#include <kernel/time/ktime.h>
#include <kernel/thread.h>
#include <kernel/thread/sync/mutex.h>

#include <drivers/serial/stm_usart.h>
#include <framework/mod/options.h>

#define AGENT_ID OPTION_GET(NUMBER, agent_id)
#define UART_NUM	2
#define MSG_LEN		1

static Led_TypeDef leds[] = { 0, 2, 4, 6, 7, 5, 3, 1 };

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

struct mutex led_mutex;

static int leds_cnt = 0;

static void leds_next(void) {
	mutex_lock(&led_mutex);
	if (++leds_cnt == sizeof(leds) + 1) {
		leds_cnt = 0;
		leds_off();
	} else {
		BSP_LED_On(leds[leds_cnt]);
	}
	mutex_unlock(&led_mutex);
}

static void leds_prev(void) {
	mutex_lock(&led_mutex);
	BSP_LED_Off(leds[leds_cnt]);
	if (--leds_cnt < 0)
		leds_cnt = 0;

	mutex_unlock(&led_mutex);
}

static int current_state;

static USART_TypeDef *uart_base[] = {
	(void*) USART1,
	(void*) USART2,
	(void*) USART3
};

extern void schedule();
static void transmit_delay(void) {
	int t = 0x3FFFFF / UART_NUM;
	while(t--);
	schedule();
}

static int obtain_data(void) {
	mutex_lock(&led_mutex);
	current_state = leds_cnt;
	mutex_unlock(&led_mutex);
	return current_state;
}

static void transmit_data(int data, USART_TypeDef *uart) {
	while ((STM32_USART_FLAGS(uart) & USART_FLAG_TXE) == 0);

	STM32_USART_TXDATA(uart) = (uint8_t) (data + (int)'0');
}

static void *transmitter_thread_run(void *arg) {
	int i;
	int data;

	while (1) {
		data = obtain_data();
		for (i = 0; i < UART_NUM; i++) {
			transmit_data(AGENT_ID, uart_base[i]);
			transmit_delay();
			transmit_data(data, uart_base[i]);
			transmit_delay();
		}
	}

	return NULL;
}

static void process_message(char *msg) {
	/* TODO */
	mutex_lock(&led_mutex);
	if (leds_cnt < *msg)
		leds_next();
	else if (leds_cnt > *msg)
		leds_prev();
	mutex_unlock(&led_mutex);
}

static int message_valid(char *msg) {
	return 1;
}

static void *receiver_thread_run(void *arg) {
	int i;
	UART_HandleTypeDef UartHandle;
	UartHandle.Instance = uart_base[0];
	char buf[UART_NUM][MSG_LEN];
	int counter[UART_NUM] = { 0 };
	
	int tt = 0;

	mutex_init(&led_mutex);

	while (1) {
		tt++;
		if (tt % 0x20 == 0) {
			tt = 1;
			if (BSP_PB_GetState(0))
				leds_next();
		}

		for (i = 0; i < UART_NUM; i++) {
			if (STM32_USART_FLAGS(uart_base[i]) & USART_FLAG_RXNE) {
				buf[i][counter[i]++] = STM32_USART_RXDATA(uart_base[i]) & 0xFF;
				if (counter[i] == MSG_LEN) {
					counter[i] = 0;
					/* Message finished */
					if (!message_valid(buf[i])) {
						counter[i] = 0;
						memset(buf[i], 0, sizeof(buf[i]));
						continue;
					} else {
						process_message(buf[i]);
					}
				}
			} else {
				__HAL_USART_CLEAR_NEFLAG(&UartHandle);
				__HAL_USART_CLEAR_FEFLAG(&UartHandle);
				__HAL_USART_CLEAR_OREFLAG(&UartHandle);
				//uart_base[i]->ICR = 0x0;
				schedule();
			}
		}
	}

	return NULL;
}

extern void HAL_UART_MspInit(UART_HandleTypeDef *);
static void init_uart(void) {
	int i;
	UART_HandleTypeDef UartHandle;

	for (i = 0; i < UART_NUM; i++) {
		memset(&UartHandle, 0, sizeof(UartHandle));
		UartHandle.Instance = uart_base[i];
		UartHandle.Init.BaudRate = 115200;
		UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
		UartHandle.Init.StopBits = UART_STOPBITS_1;
		UartHandle.Init.Parity = UART_PARITY_NONE;
		UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		UartHandle.Init.Mode = UART_MODE_TX_RX;
		HAL_UART_Init(&UartHandle);
	}
}

int main() {
	BSP_PB_Init(0, 0);
	init_leds();
	init_uart();
	//leds_next();
	leds_prev();

	thread_create(0, transmitter_thread_run, NULL);
	receiver_thread_run(NULL);

	return 0;
}
