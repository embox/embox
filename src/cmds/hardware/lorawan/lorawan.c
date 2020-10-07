/**
 * @file lorawan.c
 * @brief Si,ple diagnostic interface for RN2483
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.3
 * @date 2016-02-17
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <kernel/sched/sched_lock.h>
#include <drivers/gpio.h>
#include <drivers/serial/uart_dev.h>

#include <drivers/serial/stm_usart.h>

char cur_str[256];
char request[256];
int count;
int timeout;

static int _getc(void) {
	USART_TypeDef *uart = (void *) USARTx;
	return (uint8_t)(STM32_USART_RXDATA(uart) & 0xFF);
}

static int _putc(char ch) {
	USART_TypeDef *uart = (void *) USARTx;

	while ((STM32_USART_FLAGS(uart) & USART_FLAG_TXE) == 0);

	STM32_USART_TXDATA(uart) = (uint8_t) ch;

	return 0;
}

extern void diag_putc(char ch);

int main(int argc, char **argv) {
	char c;
	int i;
	UART_HandleTypeDef UartHandle;

	memset(&UartHandle, 0, sizeof(UartHandle));

	UartHandle.Instance = (void*) USARTx;

	UartHandle.Init.BaudRate = 57600;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits = UART_STOPBITS_1;
	UartHandle.Init.Parity = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode = UART_MODE_TX_RX;

	if (HAL_UART_Init(&UartHandle) != HAL_OK) {
		return -1;
	}
	
	printf("\n");
	while (1) {
		memset(cur_str, 0, sizeof(cur_str));
		memset(request, 0, sizeof(cur_str));

		count = 0;

		diag_putc('>');
		diag_putc(' ');

		while(1) {
			scanf("%c", &c);
			if (c == 255) {
				diag_putc('\n');
				break;
			}
			if (c == 8) {
				if (count) {
					diag_putc(' ');
					diag_putc(c);
					diag_putc(c);
					count--;
				}
				continue;
			}
			diag_putc(c);
			if (c == 255)
				break;
			request[count++] = c;
		}

		if (count == 0)
			continue;
			
		if (!strcmp(request, "exit"))
			break;

		for (i = 0; i < count; i++)
			_putc(request[i]);

		_putc('\r');
		_putc('\n');

		count = 0;
		printf("  ");
		sched_lock();
		timeout = 0xFFFFFF;
		while (timeout--) {
			if (STM32_USART_FLAGS(USARTx) & USART_FLAG_RXNE) {
				c = _getc();
				cur_str[count++] = c;
				if (c == (char)10) {
					break;
				}
			}
		}
		printf("%s", cur_str);
		sched_unlock();
	}

	return 0;
}
