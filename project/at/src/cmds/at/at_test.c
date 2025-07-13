/**
 * @file
 * @brief AT Module Test Command  
 * 
 * @date July 05, 2025
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
#include <at/emulator/at_emulator.h>
#include <at/at_parser.h>

/* PTY support */
extern int ppty(int ptyfds[2]);

static void print_usage(void) {
	printf("Usage: at_test [options]\n");
	printf("Options:\n");
	printf("  -e <port>  Run as emulator TCP server on specified port\n");
	printf("  -t         Run integration tests using PTY\n");
	printf("  -d         Run picked unit test to check return value\n");
	printf("  -h         Show this help\n");
}

/* Emulator thread data */
struct emulator_thread_data {
	int fd;
	at_emulator_t *emulator;
	volatile bool running;
};

/* Emulator thread function for PTY testing */
static void *emulator_thread_func(void *arg) {
	struct emulator_thread_data *data = (struct emulator_thread_data *)arg;
	char buffer[256];

	printf("[EMU] Emulator thread started on fd=%d\n", data->fd);

	while (data->running) {
		int n = read(data->fd, buffer, sizeof(buffer));
		if (n > 0) {
			printf("[EMU] Received %d bytes: ", n);
			for (int i = 0; i < n && i < 20; i++) {
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
			if (n > 20) {
				printf("...");
			}
			printf("\n");

			for (int i = 0; i < n; i++) {
				const char *response = at_emulator_process(data->emulator,
				    &buffer[i], 1);
				if (response && *response) {
					size_t resp_len = strlen(response);
					printf("[EMU] Sending response (%zu bytes): ", resp_len);
					for (size_t j = 0; j < resp_len && j < 50; j++) {
						if (response[j] >= 32 && response[j] < 127) {
							printf("%c", response[j]);
						}
						else if (response[j] == '\r') {
							printf("\\r");
						}
						else if (response[j] == '\n') {
							printf("\\n");
						}
						else {
							printf("\\x%02X", (unsigned char)response[j]);
						}
					}
					printf("\n");

					if (write(data->fd, response, resp_len) < 0) {
						printf("[EMU] Write error: %s\n", strerror(errno));
						break;
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

/* Run integration tests */
static int run_integration_tests(void) {
	int ptyfds[2];
	pthread_t emu_thread;
	struct emulator_thread_data emu_data;
	at_client_t client;
	char response[256];
	int test_passed = 0;
	int test_failed = 0;

	printf("=== AT Module Integration Test ===\n");
	printf("Creating PTY pair for testing...\n");

	if (ppty(ptyfds) != 0) {
		printf("ERROR: Failed to create PTY pair\n");
		return -1;
	}

	printf("PTY pair created: master=%d, slave=%d\n", ptyfds[0], ptyfds[1]);

	fcntl(ptyfds[0], F_SETFL, fcntl(ptyfds[0], F_GETFL, 0) | O_NONBLOCK);
	fcntl(ptyfds[1], F_SETFL, fcntl(ptyfds[1], F_GETFL, 0) | O_NONBLOCK);

	/* Configure PTY slave termios */
	struct termios tty;
	if (tcgetattr(ptyfds[1], &tty) == 0) {
		/* Clear all character conversions */
		tty.c_iflag &= ~(ICRNL | IGNCR | INLCR);
		tty.c_oflag &= ~(ONLCR | OCRNL | ONLRET);
		tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		tty.c_cc[VMIN] = 0;
		tty.c_cc[VTIME] = 0;
		tcsetattr(ptyfds[1], TCSANOW, &tty);
	}

	emu_data.emulator = at_emulator_create();
	if (!emu_data.emulator) {
		printf("ERROR: Failed to create AT emulator\n");
		close(ptyfds[0]);
		close(ptyfds[1]);
		return -1;
	}

	at_emulator_set_device_info(emu_data.emulator, "Test Device", "Embox",
	    "TEST-1", "1.0.0");

	emu_data.fd = ptyfds[1];
	emu_data.running = true;

	printf("Starting emulator thread...\n");
	if (pthread_create(&emu_thread, NULL, emulator_thread_func, &emu_data) != 0) {
		printf("ERROR: Failed to create emulator thread\n");
		at_emulator_destroy(emu_data.emulator);
		close(ptyfds[0]);
		close(ptyfds[1]);
		return -1;
	}

	printf("Initializing AT client...\n");
	if (at_client_init_fd(&client, ptyfds[0]) != 0) {
		printf("ERROR: Failed to initialize AT client\n");
		emu_data.running = false;
		pthread_join(emu_thread, NULL);
		at_emulator_destroy(emu_data.emulator);
		close(ptyfds[0]);
		close(ptyfds[1]);
		return -1;
	}

	usleep(10000);

	printf("\n--- Starting Tests ---\n");

	/* Test 1: Basic AT command */
	printf("\nTest 1: Basic AT command\n");
	printf("[CLIENT] Sending: AT\n");
	at_result_t result = at_client_send(&client, "AT", response,
	    sizeof(response), 1000);
	if (result == AT_OK) {
		printf("[CLIENT] Response: %s\n", response);
		printf("Test 1: PASSED\n");
		test_passed++;
	}
	else {
		printf("[CLIENT] Failed with result: %d\n", result);
		printf("Test 1: FAILED\n");
		test_failed++;
	}

	/* Test 2: ATI command */
	printf("\nTest 2: ATI command (device info)\n");
	printf("[CLIENT] Sending: ATI\n");
	result = at_client_send(&client, "ATI", response, sizeof(response), 1000);
	if (result == AT_OK) {
		printf("[CLIENT] Response:\n%s\n", response);
		if (strstr(response, "Test Device") && strstr(response, "Embox")) {
			printf("Test 2: PASSED\n");
			test_passed++;
		}
		else {
			printf("Test 2: FAILED - unexpected response\n");
			test_failed++;
		}
	}
	else {
		printf("[CLIENT] Failed with result: %d\n", result);
		printf("Test 2: FAILED\n");
		test_failed++;
	}

	/* Test 3: AT+GMR command */
	printf("\nTest 3: AT+GMR command (version)\n");
	printf("[CLIENT] Sending: AT+GMR\n");
	result = at_client_send(&client, "AT+GMR", response, sizeof(response), 1000);
	if (result == AT_OK) {
		printf("[CLIENT] Response: %s\n", response);
		if (strstr(response, "1.0.0")) {
			printf("Test 3: PASSED\n");
			test_passed++;
		}
		else {
			printf("Test 3: FAILED - unexpected version\n");
			test_failed++;
		}
	}
	else {
		printf("[CLIENT] Failed with result: %d\n", result);
		printf("Test 3: FAILED\n");
		test_failed++;
	}

	/* Test 4: Invalid command */
	printf("\nTest 4: Invalid command\n");
	printf("[CLIENT] Sending: AT+INVALID\n");
	result = at_client_send(&client, "AT+INVALID", response, sizeof(response),
	    1000);
	if (result == AT_ERROR) {
		printf("[CLIENT] Got expected ERROR\n");
		printf("Test 4: PASSED\n");
		test_passed++;
	}
	else {
		printf("[CLIENT] Unexpected result: %d\n", result);
		printf("Test 4: FAILED\n");
		test_failed++;
	}

	/* Test 5: Test command format */
	printf("\nTest 5: Test command format (AT+GMR=?)\n");
	printf("[CLIENT] Sending: AT+GMR=?\n");
	result = at_client_send(&client, "AT+GMR=?", response, sizeof(response), 1000);
	if (result == AT_OK) {
		printf("[CLIENT] Response: %s\n", response);
		printf("Test 5: PASSED\n");
		test_passed++;
	}
	else {
		printf("[CLIENT] Failed with result: %d\n", result);
		printf("Test 5: FAILED\n");
		test_failed++;
	}

	/* Test 6: Echo control */
	printf("\nTest 6: Echo control (ATE0)\n");
	printf("[CLIENT] Sending: ATE0\n");
	result = at_client_send(&client, "ATE0", response, sizeof(response), 1000);
	if (result == AT_OK) {
		printf("[CLIENT] Echo disabled\n");
		printf("Test 6: PASSED\n");
		test_passed++;
	}
	else {
		printf("[CLIENT] Failed with result: %d\n", result);
		printf("Test 6: FAILED\n");
		test_failed++;
	}

	/* Test 7: Verbose control */
	printf("\nTest 7: Verbose control (ATV0)\n");
	printf("[CLIENT] Sending: ATV0\n");
	result = at_client_send(&client, "ATV0", response, sizeof(response), 1000);
	if (result == AT_OK) {
		printf("[CLIENT] Numeric mode set\n");
		printf("Test 7: PASSED\n");
		test_passed++;
	}
	else {
		printf("[CLIENT] Failed with result: %d\n", result);
		printf("Test 7: FAILED\n");
		test_failed++;
	}

	/* Test 8: Reset control */
	printf("\nTest 8: Reset control\n");
	printf("[CLIENT] Sending: ATE0\n");
	at_client_send(&client, "ATE0", response, sizeof(response), 1000);
	printf("[CLIENT] Sending: ATV0\n");
	at_client_send(&client, "ATV0", response, sizeof(response), 1000);
	printf("Remember no echo now, and emu would return numeric responses\n");
	printf("[CLIENT] Sending: ATZ\n");
	result = at_client_send(&client, "ATZ", response, sizeof(response), 1000);
	if (result == AT_OK) {
		printf("[CLIENT] Device reset successful\n");
		printf("Test 8: PASSED\n");
		test_passed++;
	}
	else {
		printf("[CLIENT] Failed with result: %d\n", result);
		printf("Test 8: FAILED\n");
		test_failed++;
	}
	at_client_send(&client, "AT", response, sizeof(response), 1000);

	printf("\n--- Test Summary ---\n");
	printf("Total tests: %d\n", test_passed + test_failed);
	printf("Passed: %d\n", test_passed);
	printf("Failed: %d\n", test_failed);

	printf("\nCleaning up...\n");

	emu_data.running = false;
	pthread_join(emu_thread, NULL);

	at_client_close(&client);

	at_emulator_destroy(emu_data.emulator);

	close(ptyfds[0]);
	close(ptyfds[1]);

	printf("Integration test completed\n");

	return (test_failed == 0) ? 0 : -1;
}

static int run_debug_tests(void) {
	at_emulator_t *emu = at_emulator_create();
	const char *resp;
	const char *cmd = "ATE0";

	resp = at_emulator_send_cmd(emu, cmd);
	printf("%s response length: %zu\n", cmd, strlen(resp));
	printf("%s response chars:\n", cmd);
	for (int i = 0; resp[i] != '\0'; i++) {
		unsigned char ch = (unsigned char)resp[i];
		if (ch == '\r') {
			printf("  [%d]: 0x%02X (\\r)\n", i, ch);
		}
		else if (ch == '\n') {
			printf("  [%d]: 0x%02X (\\n)\n", i, ch);
		}
		else if (isprint(ch)) {
			printf("  [%d]: 0x%02X ('%c')\n", i, ch, ch);
		}
		else {
			printf("  [%d]: 0x%02X (non-printable)\n", i, ch);
		}
	}
	printf("%s response raw: '", cmd);
	for (int i = 0; resp[i] != '\0'; i++) {
		unsigned char ch = (unsigned char)resp[i];
		if (ch == '\r')
			printf("\\r");
		else if (ch == '\n')
			printf("\\n");
		else
			printf("%c", ch);
	}
	printf("'\n");
	return 0;
}

static int run_emulator(int port) {
	at_emulator_t *emulator;
	char buffer[256];
	int server_fd, client_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len;
	int opt = 1;

	printf("Starting AT emulator TCP server on port %d\n", port);

	emulator = at_emulator_create();
	if (!emulator) {
		printf("Failed to create AT emulator\n");
		return -1;
	}

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		printf("Failed to create socket: %s\n", strerror(errno));
		at_emulator_destroy(emulator);
		return -1;
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		printf("Warning: setsockopt SO_REUSEADDR failed: %s\n", strerror(errno));
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Failed to bind to port %d: %s\n", port, strerror(errno));
		close(server_fd);
		at_emulator_destroy(emulator);
		return -1;
	}

	if (listen(server_fd, 1) < 0) {
		printf("Failed to listen: %s\n", strerror(errno));
		close(server_fd);
		at_emulator_destroy(emulator);
		return -1;
	}

	printf("AT Emulator TCP server listening on port %d\n", port);
	printf("Press Ctrl+C to exit\n\n");

	while (1) {
		client_len = sizeof(client_addr);
		client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

		if (client_fd < 0) {
			if (errno == EINTR) {
				continue;
			}
			printf("Accept failed: %s\n", strerror(errno));
			continue;
		}

		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
		printf("Client connected from %s:%d\n", client_ip,
		    ntohs(client_addr.sin_port));

		const char *welcome = "AT Command Emulator Ready\r\n";
		send(client_fd, welcome, strlen(welcome), 0);

		while (1) {
			int n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

			if (n < 0) {
				if (errno == EINTR) {
					continue;
				}
				printf("Receive error: %s\n", strerror(errno));
				break;
			}
			else if (n == 0) {
				printf("Client disconnected\n");
				break;
			}

			printf("[RX %d bytes]: ", n);
			for (int i = 0; i < n && i < 20; i++) {
				if (buffer[i] >= 32 && buffer[i] <= 126) {
					printf("%c", buffer[i]);
				}
				else {
					printf("\\x%02X", (unsigned char)buffer[i]);
				}
			}
			if (n > 20) {
				printf("...");
			}
			printf("\n");

			for (int i = 0; i < n; i++) {
				const char *response = at_emulator_process(emulator, &buffer[i], 1);
				if (response && *response) {
					if (send(client_fd, response, strlen(response), 0) < 0) {
						printf("Send error: %s\n", strerror(errno));
						goto client_done;
					}
				}
			}
		}

client_done:
		close(client_fd);
		printf("Client connection closed\n\n");
	}

	close(server_fd);
	at_emulator_destroy(emulator);

	return 0;
}

int main(int argc, char *argv[]) {
	int opt;
	int tcp_port = 0;
	enum { MODE_HELP, MODE_EMULATOR, MODE_TEST, MODE_DEBUG } mode = MODE_HELP;

	while ((opt = getopt(argc, argv, "e:tdh")) != -1) {
		switch (opt) {
		case 'e':
			mode = MODE_EMULATOR;
			tcp_port = atoi(optarg);
			if (tcp_port <= 0 || tcp_port > 65535) {
				printf("Error: Invalid port number: %s\n", optarg);
				printf("Port must be between 1 and 65535\n");
				return -1;
			}
			break;
		case 't':
			mode = MODE_TEST;
			break;
		case 'd':
			mode = MODE_DEBUG;
			break;
		case 'h':
			mode = MODE_HELP;
			break;
		default:
			print_usage();
			return -1;
		}
	}

	switch (mode) {
	case MODE_EMULATOR:
		return run_emulator(tcp_port);
	case MODE_TEST:
		return run_integration_tests();
	case MODE_DEBUG:
		return run_debug_tests();
	case MODE_HELP:
		print_usage();
		return 0;
	default:
		print_usage();
		return -1;
	}
}