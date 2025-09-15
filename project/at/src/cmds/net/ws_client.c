/**
 * @file
 * @brief WebSocket Client Test Command
 * 
 * @date Sept 10, 2025
 * @author Peize Li
 */

#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include <net/lib/websocket.h>

static ws_connection_t *g_ws = NULL;
static volatile int g_running = 1;
static int g_echo_mode = 0;

/* WebSocket callbacks */
static void on_open(ws_connection_t *ws) {
	(void)ws;
	printf("WebSocket connected\n");
}

static void on_message(ws_connection_t *ws, const uint8_t *data, size_t len,
    ws_opcode_t type) {
	if (type == WS_OP_TEXT) {
		printf("Received text: %.*s\n", (int)len, data);
	}
	else {
		printf("Received binary: %zu bytes\n", len);
	}

	if (g_echo_mode) {
		/* Skip echoing back echo messages */
		if (strncmp((char *)data, "Echo: ", 6) == 0) {
			return;
		}
		printf("Echo: %.*s\n", (int)len, data);
		if (type == WS_OP_TEXT) {
			ws_send_text(ws, (const char *)data);
		}
		else {
			ws_send_binary(ws, data, len);
		}
	}
}

static void on_close(ws_connection_t *ws, uint16_t code, const char *reason) {
	(void)ws;
	printf("WebSocket closed: %u %s\n", code, reason ? reason : "");
	g_running = 0;
}

static void on_error(ws_connection_t *ws, const char *error) {
	(void)ws;
	printf("WebSocket error: %s\n", error);
}

static void on_ping(ws_connection_t *ws, const uint8_t *data, size_t len) {
	(void)ws;
	(void)data;
	printf("Received ping: %zu bytes\n", len);
}

static void on_pong(ws_connection_t *ws, const uint8_t *data, size_t len) {
	(void)ws;
	(void)data;
	printf("Received pong: %zu bytes\n", len);
}

static void signal_handler(int sig) {
	(void)sig;
	g_running = 0;
	if (g_ws) {
		ws_close(g_ws, WS_CLOSE_NORMAL, "User interrupt");
	}
}

static void print_usage(const char *prog) {
	printf("Usage: %s [options] <url>\n", prog);
	printf("Options:\n");
	printf("  -m <message>  Send message after connecting\n");
	printf("  -p            Send ping every 5 seconds\n");
	printf("  -e            Echo received messages back\n");
	printf("  -h            Show this help\n");
	printf("\nExample:\n");
	printf("  %s -m \"Hello WebSocket!\" ws://echo.websocket.org/\n", prog);
}

int main(int argc, char **argv) {
	const char *url = NULL;
	const char *message = NULL;
	int send_ping = 0;
	int opt;

	while ((opt = getopt(argc, argv, "m:peh")) != -1) {
		switch (opt) {
		case 'm':
			message = optarg;
			break;
		case 'p':
			send_ping = 1;
			break;
		case 'e':
			g_echo_mode = 1;
			break;
		case 'h':
			print_usage(argv[0]);
			return 0;
		default:
			fprintf(stderr, "Error: Unknown option '-%c'\n", optopt);
			print_usage(argv[0]);
			return 1;
		}
	}

	/* URL should be the first non-option argument */
	if (optind >= argc) {
		fprintf(stderr, "Error: URL is required\n");
		print_usage(argv[0]);
		return 1;
	}

	url = argv[optind];

	if (strncmp(url, "ws://", 5) != 0 && strncmp(url, "wss://", 6) != 0) {
		fprintf(stderr, "Error: Invalid URL scheme (must be ws:// or "
		                "wss://)\n");
		fprintf(stderr, "Got: %s\n", url);
		return 1;
	}

	printf("Connecting to %s...\n", url);
	if (message) {
		printf("Will send message: %s\n", message);
	}
	if (send_ping) {
		printf("Auto-ping enabled (every 5 seconds)\n");
	}
	if (g_echo_mode) {
		printf("Echo mode enabled\n");
	}

	ws_callbacks_t callbacks = {.on_open = on_open,
	    .on_message = on_message,
	    .on_close = on_close,
	    .on_error = on_error,
	    .on_ping = on_ping,
	    .on_pong = on_pong};

	/* Connect to WebSocket server */
	g_ws = ws_connect(url, &callbacks);
	if (!g_ws) {
		fprintf(stderr, "Failed to connect to %s\n", url);
		return 1;
	}

	if (message) {
		printf("Sending: %s\n", message);
		ws_send_text(g_ws, message);
	}

	/* Setup signal handler */
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	/* Main loop */
	int ping_counter = 0;
	printf("Connected. Type messages to send, /quit to exit\n");

	while (g_running && g_ws->state != WS_STATE_CLOSED) {
		/* Poll for incoming data */
		ws_poll(g_ws);

		/* Check connection state */
		if (g_ws->state == WS_STATE_CLOSED) {
			printf("Connection closed in ws_poll\n");
			break;
		}

		if (send_ping && ++ping_counter >= 50) {
			ping_counter = 0;
			printf("Sending ping...\n");
			ws_send_ping(g_ws, (uint8_t *)"ping", 4);
		}

		/* Check for stdin input */
		fd_set rfds;
		struct timeval tv = {0, 100000}; /* 100ms timeout */
		FD_ZERO(&rfds);
		FD_SET(STDIN_FILENO, &rfds);

		if (select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv) > 0) {
			char buffer[1024];
			if (fgets(buffer, sizeof(buffer), stdin)) {
				size_t len = strlen(buffer);
				if (len > 0 && buffer[len - 1] == '\n') {
					buffer[len - 1] = '\0';
				}

				if (strcmp(buffer, "/quit") == 0) {
					break;
				}

				printf("Sending: %s\n", buffer);
				ws_send_text(g_ws, buffer);
			}
		}
	}

	/* Cleanup */
	ws_free(g_ws);
	printf("Disconnected\n");

	return 0;
}
