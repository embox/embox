/**
 * @file
 * @brief Transmit info to port
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-30
 */

#include <unistd.h>

#include <drivers/serial/stm_usart.h>
#include <feather/libbutton.h>
#include <feather/servo.h>
#include <framework/mod/options.h>
#include <kernel/thread.h>
#include <kernel/thread/sync/mutex.h>
#include <lib/leddrv.h>

#define AGENT_ID OPTION_GET(NUMBER, agent_id)

#define UART_NUM 3
#define MSG_LEN  4

static int current_state[UART_NUM + 1];

static void leds_off(void) {
	int i;

	for (i = 0; i < LEDDRV_LED_N; i++) {
		leddrv_led_off(i);
	}
}

static void init_leds(void) {
	leds_off();
}

struct mutex led_mutex;

static int leds_cnt = 0;

static void leds_next(void) {
	mutex_lock(&led_mutex);
	if (++leds_cnt == LEDDRV_LED_N + 1) {
		leds_cnt = 0;
		leds_off();
	}
	else {
		leddrv_led_on(leds_cnt);
	}
	mutex_unlock(&led_mutex);

	current_state[UART_NUM] = leds_cnt;

	servo_set(leds_cnt * 100 / 8);
}

static void leds_prev(void) {
	mutex_lock(&led_mutex);
	leddrv_led_off(leds_cnt);
	if (--leds_cnt < 0) {
		leds_cnt = 0;
	}
	mutex_unlock(&led_mutex);

	current_state[UART_NUM] = leds_cnt;

	servo_set(leds_cnt * 100 / 8);
}

static USART_TypeDef *uart_base[] = {(void *)USART1, (void *)USART2,
    (void *)USART3};

extern void schedule();
static void transmit_delay(void) {
	int t = 0x16FF / UART_NUM;
	while (t--) {
		schedule();
	}
}

static double get_data(void) {
	double res = current_state[UART_NUM];
	int cnt = 1;
	int i;

	for (i = 0; i < UART_NUM; i++)
		if (current_state[i] > 0) {
			res += current_state[i];
			cnt++;
		}

	return res / cnt;
}

static int obtain_data(void) {
	return leds_cnt;
}

static void transmit_data(int data, USART_TypeDef *uart) {
	while ((STM32_USART_FLAGS(uart) & USART_FLAG_TXE) == 0)
		;

	STM32_USART_TXDATA(uart) = (uint8_t)(data + 0);
}

static void *transmitter_thread_run(void *arg) {
	int i, j;
	int data;
	char tx_buf[MSG_LEN] = {0xFF, AGENT_ID, 0, 0xAA};

	while (1) {
		data = obtain_data();
		for (i = 0; i < UART_NUM; i++) {
			tx_buf[2] = data;
			for (j = 0; j < MSG_LEN; j++) {
				transmit_data(tx_buf[j], uart_base[i]);
				transmit_delay();
			}
		}
	}

	return NULL;
}

static int from_neighbour(char *msg) {
	return msg[0] == 0xFF;
}

static void process_message(char *msg) {
	char tx_buf[MSG_LEN] = {0xFE, msg[1], msg[2], 0xAA};
	int i;
	if (from_neighbour(msg)) {
		mutex_lock(&led_mutex);
		current_state[msg[1]] = msg[2];
		if (get_data() > leds_cnt) {
			leds_next();
		}
		else if (get_data() < leds_cnt) {
			leds_prev();
		}
		mutex_unlock(&led_mutex);
	}

	for (i = 0; i < MSG_LEN; i++) {
		transmit_data(tx_buf[i], uart_base[0]);
		transmit_delay();
	}
}

static int message_valid(char *msg) {
	return 1;
}

static void *receiver_thread_run(void *arg) {
	int i;
	UART_HandleTypeDef UartHandle;
	UartHandle.Instance = uart_base[0];
	char buf[UART_NUM][MSG_LEN];
	int counter[UART_NUM] = {0};

	int tt = 0;

	mutex_init(&led_mutex);

	while (1) {
		tt++;
		if (tt % 0x180 == 0) {
			tt = 1;
			if (libbutton_get_state()) {
				leds_next();
			}
		}

		for (i = 0; i < UART_NUM; i++) {
			if (STM32_USART_FLAGS(uart_base[i]) & USART_FLAG_RXNE) {
				buf[i][counter[i]++] = STM32_USART_RXDATA(uart_base[i]) & 0xFF;
				if (buf[i][counter[i] - 1] == 0xFF) {
					counter[i] = 1;
					continue;
				}
				if (counter[i] == MSG_LEN) {
					counter[i] = 0;
					/* Message finished */
					if (!message_valid(buf[i])) {
						counter[i] = 0;
						memset(buf[i], 0, sizeof(buf[i]));
						continue;
					}
					else {
						//buf[i][0] = i;
						process_message(buf[i]);
					}
				}
			}
			else {
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
	int i;

	libbutton_init();

	init_leds();
	servo_init();
	init_uart();
	//leds_next();
	leds_prev();

	for (i = 0; i < UART_NUM; i++) {
		current_state[i] = -1;
	}

	current_state[UART_NUM] = 0;

	thread_create(0, transmitter_thread_run, NULL);
	receiver_thread_run(NULL);

	return 0;
}
