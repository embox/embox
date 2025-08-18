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

					/* For large responses, send in chunks to avoid PTY buffer limits */
					if (resp_len > 100) {
						size_t sent = 0;
						while (sent < resp_len) {
							/* Find next line boundary */
							size_t chunk_end = sent + 64;
							if (chunk_end < resp_len) {
								/* Look for nearest \n */
								while (chunk_end > sent
								       && response[chunk_end] != '\n') {
									chunk_end--;
								}
								if (chunk_end > sent
								    && response[chunk_end] == '\n') {
									chunk_end++; /* Include \n */
								}
								else {
									chunk_end = sent + 64; /* Use fixed size if no \n found */
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
								usleep(10000); /* 10ms delay between chunks */
							}
						}
					}
					else {
						/* Small responses can be sent at once */
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
	struct wiphy *wiphy = NULL;
	struct cfg80211_registered_device *rdev = NULL;

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

	/* Test 1: Initialize driver */
	printf("\n1. Initialize WiFi driver\n");
	wiphy = esp32c3_wifi_init_fd("wlan0", ptyfds[0]);
	if (wiphy) {
		printf("✓ Driver initialized successfully\n");
		rdev = wiphy_to_rdev(wiphy); /* Get rdev */
	}
	else {
		printf("✗ Driver initialization failed\n");
		goto cleanup;
	}

	struct wifi_core *wifi = wiphy_to_wifi(wiphy);

	/* Test 2: Scan for APs */
	printf("\n2. Scan for access points\n");

	if (rdev && rdev->ops && rdev->ops->scan) { /* Access ops through rdev */
		struct cfg80211_scan_request scan_req = {0};
		int scan_ret = rdev->ops->scan(wiphy, &scan_req);
		printf("Scan operation returned: %d\n", scan_ret);

		if (scan_ret == 0) {
			/* Wait for scan to complete */
			usleep(500000); /* 500ms */

			/* Access scan results from wifi->scan cache */
			if (wifi->scan.count > 0) {
				printf("Found %d access points\n", wifi->scan.count);
				printf("\nAvailable networks:\n");
				for (int i = 0; i < wifi->scan.count; i++) {
					struct wlan_scan_ap *ap = &wifi->scan.aps[i];
					printf("  [%d] SSID: %-20s RSSI: %3d dBm  CH: %2d  "
					       "Encryption: %s\n",
					    i, ap->ssid, ap->rssi, ap->channel,
					    ap->enc_type == 0   ? "Open"
					    : ap->enc_type == 1 ? "WEP"
					    : ap->enc_type == 2 ? "WPA"
					                        : "WPA2");
				}
			}
		}
		else {
			printf("✗ Scan failed\n");
		}
	}

	/* Test 3: Connect to AP */
	printf("\n3. Connect to access point\n");
	if (rdev && rdev->ops && rdev->ops->connect && wifi->scan.count > 0) {
		const char *ssid = wifi->scan.aps[0].ssid;
		printf("Attempting connection to '%s'...\n", ssid);

		struct cfg80211_connect_params conn_params = {.ssid = (uint8_t *)ssid,
		    .ssid_len = strlen(ssid),
		    .key = (uint8_t *)"password",
		    .key_len = 8};

		int conn_ret = rdev->ops->connect(wiphy, wifi->netdev, &conn_params);
		printf("Connect operation returned: %d\n", conn_ret);

		if (conn_ret == 0) {
			/* Wait for connection - up to 2 seconds (40 * 50ms) */
			usleep(2000000);
			for (int i = 0; i < 40; i++) {
				at_client_process_rx(wifi->at_client); /* Process any pending data */

				if (wifi->wdev.conn_state == CFG80211_CONNECTED) {
					break; /* Connection successful */
				}
				usleep(50000); /* 50ms */
			}

			if (wifi->wdev.conn_state == CFG80211_CONNECTED) {
				printf("✓ Connected successfully\n");

				/* Check connection info */
				printf("\nConnection details:\n");
				printf("  SSID: %s\n", wifi->conn_info.ssid);
				printf("  IP Address: %s\n", wifi->conn_info.ip_addr);
				printf("  Gateway: %s\n", wifi->conn_info.gateway);
				printf("  Signal Strength: %d dBm\n", wifi->conn_info.rssi);
			}
		}
		else {
			printf("✗ Connection failed\n");
		}
	}

	/* Test 4: Power Management */
	printf("\n4. Test Power Management\n");
	if (wifi->wdev.conn_state == CFG80211_CONNECTED) {
		int current_mode;

		/* Get current power mode */
		if (esp32c3_get_power_mode(wiphy, &current_mode) == 0) {
			printf("Current power mode: %s\n",
			    current_mode == ESP32_POWER_ACTIVE        ? "Active"
			    : current_mode == ESP32_POWER_MODEM_SLEEP ? "Modem Sleep"
			                                              : "Light Sleep");
		}

		/* Test switching to power save mode */
		printf("Switching to Modem Sleep mode...\n");
		if (esp32c3_set_power_mode(wiphy, ESP32_POWER_MODEM_SLEEP) == 0) {
			printf("✓ Power mode changed successfully\n");

			/* Verify connection is still active */
			if (wifi->wdev.conn_state == CFG80211_CONNECTED) {
				printf("✓ Connection maintained in Power Save mode\n");
			}
		}

		/* Restore to active mode */
		esp32c3_set_power_mode(wiphy, ESP32_POWER_ACTIVE);
	}

	/* Test 5: Get ESP32-specific status */
	printf("\n5. Get ESP32 Status\n");
	struct esp32c3_status status;
	if (esp32c3_get_status(wiphy, &status) == 0) {
		printf("Module Status:\n");
		printf("  Firmware: %s\n", status.fw_version);
		printf("  MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", status.mac_addr[0],
		    status.mac_addr[1], status.mac_addr[2], status.mac_addr[3],
		    status.mac_addr[4], status.mac_addr[5]);
		printf("  Connected: %s\n", status.connected ? "Yes" : "No");
		if (status.connected) {
			printf("  SSID: %s\n", status.ssid);
			printf("  IP: %s\n", status.ip_addr);
		}
	}

	/* Test 6: Disconnect */
	printf("\n6. Disconnect from network\n");
	if (rdev && rdev->ops && rdev->ops->disconnect) { /* Access ops through rdev */
		int disc_ret = rdev->ops->disconnect(wiphy, wifi->netdev, 0);
		printf("Disconnect operation returned: %d\n", disc_ret);
		if (disc_ret == 0) {
			printf("✓ Disconnected successfully\n");
		}
	}

cleanup:
	printf("\n=== Cleanup ===\n");

	/* 1. Cleanup WiFi driver */
	if (wiphy) {
		esp32c3_wifi_cleanup(wiphy);
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