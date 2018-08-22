/**
 * @file
 * @brief  Connect two STM32F4_Discovery boards with attached
 *         NRF24L01+ modules
 *
 * @date   23.06.18
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <util/log.h>

#include "stm32f4_discovery.h"

#include <libs/nrf24.h>

#define MAX_BUF_SZ   256

#define TEST_ITERS   10

static int buffer_cmp(char *tx, char *rx, size_t sz) {
	int i;

	for (i = 0; i < sz; i++) {
		if (tx[i] != rx[i]) {
			int j, k;

			/* Print out tx and rx data */
			printf("TX:\n");
			for (k = 0; k < sz / 16; k++) {
				for (j = 0; j < 16; j++) {
					printf("%02X ", tx[16 * k + j]);
				}
				printf("\n");
			}
			printf("RX:\n");
			for (k = 0; k < sz / 16; k++) {
				for (j = 0; j < 16; j++) {
					printf("%02X ", rx[16 * k + j]);
				}
				printf("\n");
			}
			printf("\n");

			return -1;
		}
	}

	return 0;
}

/* Send and receive same data */
int nrf24_test_tx_rx(int host_nr, int payload_len) {
	int i;
	uint8_t tmp;
	int rx_i = 0, tx_i = 0;
	uint8_t tx_buf[MAX_BUF_SZ];
	uint8_t rx_buf[MAX_BUF_SZ];
	int data_sz = (MAX_BUF_SZ / payload_len) * payload_len;

	for (i = 0; i < data_sz; i++) {
		tx_buf[i] = i;
	}

	if (host_nr == 1) {
		/* First board start transmiting at first */
		goto start_tx;
	} else if (host_nr == 2) {
		/* Second board start receiving at first */
		goto start_rx;
	}

	/* Send and receive data_sz amount of data */
	while (1) {
start_tx:
		while (tx_i < data_sz) {
			nrf24_send(&tx_buf[tx_i]);
			while (nrf24_isSending())
				;
			tmp = nrf24_lastMessageStatus();
			if (tmp == NRF24_TRANSMISSON_OK) {
				tx_i += payload_len;
				log_debug("TX (tx_i=%d,rx_i=%d,data_sz=%d)\n",
						tx_i, rx_i, data_sz);
			} else {
				usleep(10000);
			}
		}

start_rx:
		nrf24_powerUpRx();
		while (rx_i < data_sz) {
			while (!nrf24_dataReady())
				;
			nrf24_getData(&rx_buf[rx_i]);

			/* Check if this packet is not equal not the previous one */
			/* TODO Investigate why during NRF24 module's start in RX mode
			 * we can get two identical packets from the transmitter.
			 * To reproduce this behaviour, first start TX, then start RX. */
			if (rx_i < payload_len ||
					(rx_buf[rx_i] != rx_buf[rx_i - payload_len])) {
				rx_i += payload_len;
				log_debug("RX (tx_i=%d,rx_i=%d,data_sz=%d)\n",
					tx_i, rx_i, data_sz);
			}
		}

		/* Finish if all data were transmitted and received */
		if (tx_i == data_sz && rx_i == data_sz) {
			break;
		}
	}

	/* Chack the data received is an expected data */
	if (buffer_cmp((char *)tx_buf, (char *)rx_buf, data_sz) < 0) {
		printf("Error: NRF24L01 rx and tx buffers comparision error\n");
		return -1;
	}

	return 0;
}

static int nrf24_test(int host_nr) {
	uint8_t addr1[5] = {0xAA,0xBB,0xCC,0xDD,0x01};
	uint8_t addr2[5] = {0xEE,0xFF,0xAA,0xBB,0x02};
	uint8_t payload_len;
	int i;

	/* Init hardware pins and SPI, setup irq handler */
	nrf24_init();

	/* Test for different payloads */
	for (payload_len = 4; payload_len <= NRF24_MAX_PAYLOAD; payload_len++) {
		if (nrf24_config(96, payload_len) < 0) {
			printf("Error: NRF24L01 module seems to be disconnected\n");
			return -1;
		}

		switch (host_nr) {
		case 1:
			nrf24_rx_address(addr1);
			nrf24_tx_address(addr2);
			break;
		case 2:
			nrf24_rx_address(addr2);
			nrf24_tx_address(addr1);
			break;
		default:
			return -1;
		}

		for (i = 0; i < TEST_ITERS; i++) {
			printf("\r test for payload = %d (%d of %d)",
					payload_len, i + 1, TEST_ITERS);
			if (nrf24_test_tx_rx(host_nr, payload_len) < 0) {
				return -1;
			}
		}
		printf("\n");
	}

	return 0;
}

static void init_leds() {
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
}

static void print_usage(void) {
	printf("Usage: nrf24_connect_boards [-h] -c <host_nr>\n"
			"Example:\n"
			"  First board: nrf24_connect_boards -c 1\n"
			"  Second board: nrf24_connect_boards -c 2\n");
}

int main(int argc, char *argv[]) {
	int host_nr;
	int opt;

	init_leds();

	if (argc <= 1) {
		print_usage();
		return -EINVAL;
	}

	while (-1 != (opt = getopt(argc, argv, "hc:"))) {
		printf("\n");
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'c':
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &host_nr))) {
				print_usage();
				return -EINVAL;
			}
			break;
		default:
			print_usage();
			return -EINVAL;
		}
	}

	if (nrf24_test(host_nr) < 0) {
		printf("ERROR\n");
		BSP_LED_Toggle(LED5);
	} else {
		BSP_LED_Toggle(LED4);
		printf("SUCCESS\n");
	}

	return 0;
}
