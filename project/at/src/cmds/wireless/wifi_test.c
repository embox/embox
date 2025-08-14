/**
 * @file
 * @brief WiFi Module Test Command
 * 
 * @date Aug 6, 2025
 * @author Peize Li
 */

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>

#include <at/at_client.h>
#include <at/at_parser.h>
#include <at/emulator/at_emulator.h>
#include <net/netdevice.h>
#include <net/wlan/wireless_ioctl.h>
#include <wifi/esp32c3/emulator/esp32c3_emulator.h>
#include <wifi/esp32c3/esp32c3_wifi.h>
#include <wifi/wifi_core.h>

/* PTY support */
extern int ppty(int ptyfds[2]);

/* Emulator thread data */
struct emulator_thread_data {
	int fd;
	at_emulator_t *emulator;
	volatile bool running;
};

static void print_usage(void) {
	printf("Usage: wifi_test [options]\n");
	printf("Options:\n");
	printf("  -t         Run WiFi integration tests\n");
	printf("  -h         Show this help\n");
}

/* Emulator thread function for PTY testing */
static void *emulator_thread_func(void *arg) {
	struct emulator_thread_data *data = (struct emulator_thread_data *)arg;
	char buffer[256];

	printf("[EMU] Emulator thread started on fd=%d\n", data->fd);

	while (data->running) {
		int n = read(data->fd, buffer, sizeof(buffer));
		if (n > 0) {
			printf("[EMU] Received %d bytes: ", n);
			for (int i = 0; i < n && i < 50; i++) {
				if (buffer[i] >= 32 && buffer[i] < 127) {
					printf("%c", buffer[i]);
				}
				else if (buffer[i] == '\r') {
					printf("\\r");
				}
				else if (buffer[i] == '\n') {
					printf("\\n");
				}
				else {
					printf("\\x%02X", (unsigned char)buffer[i]);
				}
			}
			if (n > 50) {
				printf("...");
			}
			printf("\n");

			for (int i = 0; i < n; i++) {
				const char *response = at_emulator_process(data->emulator,
				    &buffer[i], 1);
				if (response && *response) {
					size_t resp_len = strlen(response);
					printf("[EMU] Sending response (%zu bytes)\n", resp_len);

					// For large responses, send in chunks to avoid PTY buffer limits
					if (resp_len > 100) {
						size_t sent = 0;
						while (sent < resp_len) {
							// Find next line boundary
							size_t chunk_end = sent + 64;
							if (chunk_end < resp_len) {
								// Look for nearest \n
								while (chunk_end > sent
								       && response[chunk_end] != '\n') {
									chunk_end--;
								}
								if (chunk_end > sent
								    && response[chunk_end] == '\n') {
									chunk_end++; // Include \n
								}
								else {
									chunk_end = sent + 64; // Use fixed size if no \n found
								}
							}
							else {
								chunk_end = resp_len;
							}

							size_t to_send = chunk_end - sent;
							if (write(data->fd, response + sent, to_send) < 0) {
								printf("[EMU] Write error: %s\n", strerror(errno));
								break;
							}
							sent = chunk_end;

							if (sent < resp_len) {
								usleep(10000); // 10ms delay between chunks
							}
						}
					}
					else {
						// Small responses can be sent at once
						if (write(data->fd, response, resp_len) < 0) {
							printf("[EMU] Write error: %s\n", strerror(errno));
							break;
						}
					}
				}
			}
		}
		else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
			printf("[EMU] Read error: %s\n", strerror(errno));
			break;
		}

		usleep(1000);
	}

	printf("[EMU] Emulator thread exiting\n");
	return NULL;
}

static void *emulator_poll_thread(void *arg) {
	struct emulator_thread_data *data = (struct emulator_thread_data *)arg;

	while (data->running) {
		const char *urc = at_emulator_poll(data->emulator);
		if (urc && *urc) {
			write(data->fd, urc, strlen(urc));
		}
		usleep(50000); /* 50ms */
	}

	return NULL;
}

static int run_wifi_test(void) {
	int ptyfds[2];
	pthread_t emu_thread, poll_thread;
	struct emulator_thread_data emu_data;
	struct net_device netdev = {0};

	printf("=== WiFi Integration Test ===\n");

	/* Create PTY and emulator */
	if (ppty(ptyfds) != 0) {
		printf("Failed to create PTY\n");
		return -1;
	}

	fcntl(ptyfds[1], F_SETFL, fcntl(ptyfds[1], F_GETFL, 0) | O_NONBLOCK);

	emu_data.emulator = esp32c3_emulator_create();
	if (!emu_data.emulator) {
		printf("Failed to create ESP32-C3 emulator\n");
		close(ptyfds[0]);
		close(ptyfds[1]);
		return -1;
	}

	emu_data.fd = ptyfds[1];
	emu_data.running = true;

	pthread_create(&emu_thread, NULL, emulator_thread_func, &emu_data);
	pthread_create(&poll_thread, NULL, emulator_poll_thread, &emu_data);

	usleep(100000);

	/* Initialize network device */
	strcpy(netdev.name, "wlan0");
	netdev.flags = 0;

	/* Test 1: Initialize driver */
	printf("\n1. Initialize WiFi driver\n");
	if (esp32c3_wifi_init_fd(&netdev, ptyfds[0]) == 0) {
		printf("✓ Driver initialized successfully\n");
	}
	else {
		printf("✗ Driver initialization failed\n");
		goto cleanup;
	}

	struct wlan_device *wdev = netdev.wireless_priv;
	struct wifi_core *wifi = wdev ? wdev->priv : NULL;

	/* Test 2: Scan for APs */
	printf("\n2. Scan for access points\n");

	if (netdev.wireless_ops && netdev.wireless_ops->scan) {
		int scan_ret = netdev.wireless_ops->scan(&netdev);
		printf("Scan operation returned: %d\n", scan_ret);

		if (scan_ret == 0 && wdev) {
			printf("Found %d access points\n", wdev->scan_result.count);

			if (wdev->scan_result.count > 0) {
				printf("\nAvailable networks:\n");
				for (int i = 0; i < wdev->scan_result.count; i++) {
					struct wlan_scan_ap *ap = &wdev->scan_result.aps[i];
					printf("  [%d] SSID: %-20s RSSI: %3d dBm  CH: %2d  "
					       "Security: %s\n",
					    i, ap->ssid, ap->rssi, ap->channel,
					    ap->security ? "WPA2" : "Open");
				}
			}
		}
		else {
			printf("✗ Scan failed\n");
		}
	}

	/* Test 3: Connect to AP */
	printf("\n3. Connect to access point\n");
	if (wdev && wdev->scan_result.count > 0 && netdev.wireless_ops->connect) {
		const char *ssid = wdev->scan_result.aps[0].ssid;
		printf("Attempting connection to '%s'...\n", ssid);

		int conn_ret = netdev.wireless_ops->connect(&netdev, ssid, "password");
		printf("Connect operation returned: %d\n", conn_ret);

		if (conn_ret == 0) {
			printf("✓ Connected successfully\n");

			/* Check connection info */
			if (wifi && wifi_core_has_ip(wifi)) {
				char ip[16] = {0};
				char connected_ssid[33] = {0};
				int8_t rssi = 0;

				if (wifi_core_get_connection_info(wifi, connected_ssid, ip, &rssi)
				    == 0) {
					printf("\nConnection details:\n");
					printf("  SSID: %s\n", connected_ssid);
					printf("  IP Address: %s\n", ip);
					printf("  Signal Strength: %d dBm\n", rssi);
				}
			}
		}
		else {
			printf("✗ Connection failed\n");
		}
	}

	/* Test 4: Power Management */
	printf("\n4. Test Power Management\n");
	if (wifi && wifi_core_is_connected(wifi)) {
		int current_mode;

		// get current power mode
		if (esp32c3_get_power_mode(wifi, &current_mode) == 0) {
			printf("Current power mode: %s\n",
			    current_mode == WLAN_PWR_ACTIVE ? "Active"
			    : current_mode == WLAN_PWR_SAVE ? "Power Save"
			                                    : "Deep Save");
		}

		// test switching to power save mode
		printf("Switching to Power Save mode...\n");
		if (esp32c3_set_power_mode(wifi, WLAN_PWR_SAVE) == 0) {
			printf("✓ Power mode changed successfully\n");

			// verify connection is still active
			if (wifi_core_is_connected(wifi)) {
				printf("✓ Connection maintained in Power Save mode\n");
			}
		}

		// restore to active mode
		esp32c3_set_power_mode(wifi, WLAN_PWR_ACTIVE);
	}

	/* Test 5: Disconnect */
	printf("\n5. Disconnect from network\n");
	if (netdev.wireless_ops && netdev.wireless_ops->disconnect) {
		int disc_ret = netdev.wireless_ops->disconnect(&netdev);
		printf("Disconnect operation returned: %d\n", disc_ret);
		if (disc_ret == 0) {
			printf("✓ Disconnected successfully\n");
		}
	}

cleanup:
	printf("\n=== Cleanup ===\n");

	/* 1. Cleanup WiFi driver (while emulator is still alive) */
	if (netdev.wireless_ops) {
		esp32c3_wifi_cleanup(&netdev);
		printf("✓ WiFi driver cleaned up\n");
	}

	/* 2. Stop emulator threads */
	emu_data.running = false;
	pthread_join(emu_thread, NULL);
	pthread_join(poll_thread, NULL);
	printf("✓ Emulator threads stopped\n");

	/* 3. Cleanup emulator */
	esp32c3_emulator_destroy(emu_data.emulator);
	printf("✓ Emulator destroyed\n");

	/* 4. Close PTY */
	if (ptyfds[0] >= 0)
		close(ptyfds[0]);
	if (ptyfds[1] >= 0)
		close(ptyfds[1]);
	printf("✓ PTY closed\n");

	printf("\n=== Test Complete ===\n");
	return 0;
}

int main(int argc, char *argv[]) {
	int opt;

	/* No options provided, show usage */
	if (argc == 1) {
		print_usage();
		return 0;
	}

	while ((opt = getopt(argc, argv, "th")) != -1) {
		switch (opt) {
		case 't':
			return run_wifi_test();
		case 'h':
			print_usage();
			return 0;
		default:
			print_usage();
			return -1;
		}
	}

	print_usage();
	return -1;
}