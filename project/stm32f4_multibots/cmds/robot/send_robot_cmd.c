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

#include <libs/nrf24.h>

static void get_response(char *cmd, int payload_len) {
	uint8_t rx_buf[32];
	uint16_t light_level;

	if (strncmp(cmd, "light", 5)) {
		return;
	}

	nrf24_powerUpRx();
	while (!nrf24_dataReady())
		;
	nrf24_getData(rx_buf);

	/* Currently, only <light> waits for response */
	if (0 == strncmp(cmd, "light", 5)) {
		light_level = *(uint16_t *) rx_buf;
		printf("Received light level - %d\n", light_level);
		return;
	}
}

static void send_cmd_to_robot(char *cmd, int payload_len) {
	printf("Sending cmd - %s...\n", cmd);

	while (1) {
		int tmp = 0;

		nrf24_send((uint8_t *)cmd);
		while (nrf24_isSending())
			;
		tmp = nrf24_lastMessageStatus();
		if (tmp == NRF24_TRANSMISSON_OK) {
			printf("Transmission OK\n");
			break;
		} else {
			printf("Transmission error\n");
			usleep(500000);
		}
	}

	get_response(cmd, payload_len);
}

static void print_usage(void) {
	printf("Usage: send_robot_cmd [-h] -p <payload_len> command\n"
			"Supported commands:\n"
			"  move [up | down]\n"
			"  light\n"
			"  stop\n"
			"  start\n"
			"Example:\n"
			"  send_robot_cmd -p 16 \"move up\"\n"
			"  send_robot_cmd -p 16 \"light\"\n");
}

int main(int argc, char *argv[]) {
	int opt;
	int payload_len;
	uint8_t rx_addr[5] = {0xAA,0xBB,0xCC,0xDD,0x01};
	uint8_t tx_addr[5] = {0xEE,0xFF,0xAA,0xBB,0x02};

	if (argc <= 1) {
		print_usage();
		return -EINVAL;
	}

	while (-1 != (opt = getopt(argc, argv, "hp:"))) {
		printf("\n");
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'p':
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &payload_len))) {
				print_usage();
				return -EINVAL;
			}
			break;
		default:
			print_usage();
			return -EINVAL;
		}
	}

	nrf24_init();

	if (nrf24_config(96, payload_len) < 0) {
		return -1;
	}

	nrf24_rx_address(rx_addr);
	nrf24_tx_address(tx_addr);

	nrf24_powerUpRx();

	send_cmd_to_robot(argv[3], payload_len);

	nrf24_powerDown();

	return 0;
}
