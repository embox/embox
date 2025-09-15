/**
 * @file
 * @brief WebSocket Core Implementation
 * 
 * @date Sept 10, 2025
 * @author Peize Li
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hal/clock.h>
#include <mem/misc/pool.h>
#include <net/lib/websocket.h>

#define WS_POOL_SIZE 4
POOL_DEF(ws_connection_pool, ws_connection_t, WS_POOL_SIZE);

/* Forward declarations */
extern int ws_handshake(ws_connection_t *ws, const char *host, const char *path);
extern int ws_parse_frame_header(ws_connection_t *ws, const uint8_t *data,
    size_t len);
extern int ws_send_frame(ws_connection_t *ws, ws_opcode_t opcode,
    const uint8_t *data, size_t len);

/**
 * Parse URL into components
 */
static int ws_parse_url(const char *url, char *host, int *port, char *path) {
	const char *p;

	/* Skip ws:// or wss:// */
	if (strncmp(url, "ws://", 5) == 0) {
		p = url + 5;
		*port = 80;
	}
	else if (strncmp(url, "wss://", 6) == 0) {
		fprintf(stderr, "WSS (secure WebSocket) not supported\n");
		return -1;
	}
	else {
		fprintf(stderr, "Invalid URL scheme (must be ws://)\n");
		return -1;
	}

	/* Find the end of host part */
	const char *slash = strchr(p, '/');
	const char *colon = strchr(p, ':');

	/* Determine host end position */
	if (colon && (!slash || colon < slash)) {
		/* Host:port format */
		size_t host_len = colon - p;
		if (host_len >= 256) {
			fprintf(stderr, "Host name too long\n");
			return -1;
		}
		memcpy(host, p, host_len);
		host[host_len] = '\0';

		/* Parse port */
		char port_str[16];
		size_t port_len = (slash ? slash : p + strlen(p)) - (colon + 1);
		if (port_len >= sizeof(port_str)) {
			fprintf(stderr, "Invalid port\n");
			return -1;
		}
		memcpy(port_str, colon + 1, port_len);
		port_str[port_len] = '\0';
		*port = atoi(port_str);

		if (*port <= 0 || *port > 65535) {
			fprintf(stderr, "Invalid port number: %d\n", *port);
			return -1;
		}
	}
	else {
		/* Host only */
		size_t host_len = slash ? (slash - p) : strlen(p);
		if (host_len >= 256) {
			fprintf(stderr, "Host name too long\n");
			return -1;
		}
		memcpy(host, p, host_len);
		host[host_len] = '\0';
	}

	/* Extract path */
	if (slash) {
		strcpy(path, slash);
	}
	else {
		strcpy(path, "/");
	}

	printf("Parsed URL: host=%s, port=%d, path=%s\n", host, *port, path);
	return 0;
}

/**
 * Connect to WebSocket server
 */
ws_connection_t *ws_connect(const char *url, const ws_callbacks_t *callbacks) {
	ws_connection_t *ws;
	char host[256];
	char path[256];
	int port;
	struct sockaddr_in server_addr;
	struct hostent *server;
	int sock_fd = -1;

	printf("ws_connect: URL=%s\n", url);

	/* Parse URL */
	if (ws_parse_url(url, host, &port, path) < 0) {
		fprintf(stderr, "Failed to parse URL\n");
		return NULL;
	}

	/* Allocate connection from pool */
	ws = pool_alloc(&ws_connection_pool);
	if (!ws) {
		fprintf(stderr, "Failed to allocate connection from pool\n");
		return NULL;
	}

	memset(ws, 0, sizeof(*ws));
	ws->sock_fd = -1;
	ws->state = WS_STATE_CONNECTING;
	if (callbacks) {
		memcpy(&ws->callbacks, callbacks, sizeof(ws->callbacks));
	}

	/* Create TCP socket */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) {
		fprintf(stderr, "Failed to create socket: %s\n", strerror(errno));
		pool_free(&ws_connection_pool, ws);
		return NULL;
	}

	ws->sock_fd = sock_fd;

	/* Setup server address */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	printf("Resolving %s...\n", host);

	/* First try as IP address */
	if (inet_aton(host, &server_addr.sin_addr) == 0) {
		/* Not an IP address, try DNS */
		server = gethostbyname(host);
		if (!server) {
			fprintf(stderr, "Failed to resolve host: %s\n", host);
			close(ws->sock_fd);
			pool_free(&ws_connection_pool, ws);
			return NULL;
		}

		/* Use h_addr_list[0] */
		if (server->h_addr_list && server->h_addr_list[0]) {
			memcpy(&server_addr.sin_addr.s_addr, server->h_addr_list[0],
			    server->h_length);
		}
		else {
			fprintf(stderr, "No addresses found for host: %s\n", host);
			close(ws->sock_fd);
			pool_free(&ws_connection_pool, ws);
			return NULL;
		}
	}

	/* Connect to server */
	printf("Connecting to %s:%d...\n", inet_ntoa(server_addr.sin_addr), port);

	if (connect(ws->sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))
	    < 0) {
		fprintf(stderr, "Failed to connect: %s\n", strerror(errno));
		close(ws->sock_fd);
		pool_free(&ws_connection_pool, ws);
		return NULL;
	}

	printf("TCP connected, performing WebSocket handshake...\n");

	/* Perform WebSocket handshake */
	if (ws_handshake(ws, host, path) < 0) {
		fprintf(stderr, "WebSocket handshake failed\n");
		close(ws->sock_fd);
		pool_free(&ws_connection_pool, ws);
		return NULL;
	}

	printf("WebSocket handshake complete\n");
	return ws;
}

/**
 * Process received frame
 */
static void ws_process_frame(ws_connection_t *ws) {
	uint8_t *payload = ws->frame_buffer + ws->frame_size;
	size_t payload_len = ws->current_frame.payload_len;

	/* Unmask payload if needed (server->client is unmasked) */
	if (ws->current_frame.masked) {
		for (size_t i = 0; i < payload_len; i++) {
			payload[i] ^= ws->current_frame.mask_key[i % 4];
		}
	}

	/* Handle frame by opcode */
	switch (ws->current_frame.opcode) {
	case WS_OP_TEXT:
	case WS_OP_BINARY:
		if (ws->callbacks.on_message) {
			ws->callbacks.on_message(ws, payload, payload_len,
			    ws->current_frame.opcode);
		}
		break;

	case WS_OP_CLOSE: {
		uint16_t code = WS_CLOSE_NORMAL;
		char reason[256] = "";

		if (payload_len >= 2) {
			code = (payload[0] << 8) | payload[1];
			if (payload_len > 2) {
				size_t reason_len = payload_len - 2;
				if (reason_len > sizeof(reason) - 1) {
					reason_len = sizeof(reason) - 1;
				}
				memcpy(reason, payload + 2, reason_len);
				reason[reason_len] = '\0';
			}
		}

		/* Echo close frame if not already closing */
		if (ws->state == WS_STATE_OPEN) {
			ws->state = WS_STATE_CLOSING;
			ws_send_frame(ws, WS_OP_CLOSE, payload, payload_len);
		}

		ws->state = WS_STATE_CLOSED;

		if (ws->callbacks.on_close) {
			ws->callbacks.on_close(ws, code, reason);
		}
	} break;

	case WS_OP_PING:
		/* Auto-reply with pong */
		ws_send_frame(ws, WS_OP_PONG, payload, payload_len);

		if (ws->callbacks.on_ping) {
			ws->callbacks.on_ping(ws, payload, payload_len);
		}
		break;

	case WS_OP_PONG:
		if (ws->callbacks.on_pong) {
			ws->callbacks.on_pong(ws, payload, payload_len);
		}
		break;

	default:
		if (ws->callbacks.on_error) {
			ws->callbacks.on_error(ws, "Unknown opcode");
		}
		break;
	}

	/* Reset buffer for next frame */
	ws->buffer_pos = 0;
	ws->frame_size = 0;
}

/**
 * Poll for incoming data
 */
void ws_poll(ws_connection_t *ws) {
	struct pollfd pfd;
	uint8_t temp_buffer[4096];
	ssize_t n;

	if (!ws || ws->state == WS_STATE_CLOSED) {
		return;
	}

	pfd.fd = ws->sock_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	int poll_result = poll(&pfd, 1, 0);
	if (poll_result <= 0) {
		return;
	}

	n = recv(ws->sock_fd, temp_buffer, sizeof(temp_buffer), 0);

	if (n <= 0) {
		printf("ws_poll: closing connection (n=%d, errno=%d)\n", (int)n, errno);
		ws->state = WS_STATE_CLOSED;
		if (ws->callbacks.on_close) {
			ws->callbacks.on_close(ws, WS_CLOSE_GOING_AWAY, "Connection lost");
		}
		return;
	}

	/* Process received data */
	size_t processed = 0;
	while (processed < n) {
		if (ws->frame_size == 0) {
			/* Parse frame header */
			int header_len = ws_parse_frame_header(ws, temp_buffer + processed,
			    n - processed);

			if (header_len == 0) {
				/* Need more data for header */
				break;
			}
			else if (header_len < 0) {
				/* Protocol error */
				if (ws->callbacks.on_error) {
					ws->callbacks.on_error(ws, "Invalid frame header");
				}
				ws_close(ws, WS_CLOSE_PROTOCOL_ERROR, "Invalid frame");
				return;
			}

			ws->frame_size = header_len;
			processed += header_len;

			/* Check payload size */
			if (ws->current_frame.payload_len > WS_MAX_PAYLOAD_SIZE) {
				ws_close(ws, WS_CLOSE_MESSAGE_TOO_BIG, "Message too large");
				return;
			}
		}

		/* Copy payload data */
		size_t payload_received = ws->buffer_pos;
		size_t payload_remaining = ws->current_frame.payload_len - payload_received;
		size_t available = n - processed;
		size_t to_copy = (available < payload_remaining) ? available
		                                                 : payload_remaining;

		memcpy(ws->frame_buffer + ws->frame_size + payload_received,
		    temp_buffer + processed, to_copy);
		ws->buffer_pos += to_copy;
		processed += to_copy;

		/* Check if frame is complete */
		if (ws->buffer_pos >= ws->current_frame.payload_len) {
			ws_process_frame(ws);
		}
	}
}

/**
 * Send text message
 */
int ws_send_text(ws_connection_t *ws, const char *text) {
	if (!ws || !text) {
		return -1;
	}
	return ws_send_frame(ws, WS_OP_TEXT, (const uint8_t *)text, strlen(text));
}

/**
 * Send binary message
 */
int ws_send_binary(ws_connection_t *ws, const uint8_t *data, size_t len) {
	if (!ws || !data) {
		return -1;
	}
	return ws_send_frame(ws, WS_OP_BINARY, data, len);
}

/**
 * Send ping frame
 */
int ws_send_ping(ws_connection_t *ws, const uint8_t *data, size_t len) {
	if (!ws) {
		return -1;
	}
	return ws_send_frame(ws, WS_OP_PING, data, len);
}

/**
 * Close connection
 */
int ws_close(ws_connection_t *ws, uint16_t code, const char *reason) {
	uint8_t payload[256];
	size_t payload_len = 0;

	if (!ws || ws->state == WS_STATE_CLOSED) {
		return -1;
	}

	/* Build close payload */
	payload[0] = (code >> 8) & 0xFF;
	payload[1] = code & 0xFF;
	payload_len = 2;

	if (reason) {
		size_t reason_len = strlen(reason);
		if (reason_len > sizeof(payload) - 2) {
			reason_len = sizeof(payload) - 2;
		}
		memcpy(payload + 2, reason, reason_len);
		payload_len += reason_len;
	}

	/* Send close frame */
	ws->state = WS_STATE_CLOSING;
	ws_send_frame(ws, WS_OP_CLOSE, payload, payload_len);

	return 0;
}

/**
 * Free connection resources
 */
void ws_free(ws_connection_t *ws) {
	if (!ws) {
		return;
	}

	if (ws->sock_fd >= 0) {
		close(ws->sock_fd);
	}

	pool_free(&ws_connection_pool, ws);
}

static struct {
	uint32_t seed;
	int initialized;
} g_prng = {0, 0};

static void prng_init(void) {
	if (!g_prng.initialized) {
		/* Combine multiple entropy sources */
		g_prng.seed = 0;

		/* System clock */
		g_prng.seed ^= (uint32_t)clock_sys_ticks();

		/* Stack address randomness */
		uint32_t stack_var;
		g_prng.seed ^= (uint32_t)(uintptr_t)&stack_var;

		/* Code segment address (may differ on each load) */
		g_prng.seed ^= (uint32_t)(uintptr_t)&prng_init;

		/* Simple mixing */
		g_prng.seed = (g_prng.seed ^ 0xDEADBEEF) * 1664525 + 1013904223;

		/* Ensure non-zero */
		if (g_prng.seed == 0) {
			g_prng.seed = 0x12345678;
		}

		g_prng.initialized = 1;
	}
}

/* xorshift32 implementation */
static uint32_t prng_next(void) {
	prng_init();

	/* xorshift32 algorithm */
	g_prng.seed ^= g_prng.seed << 13;
	g_prng.seed ^= g_prng.seed >> 17;
	g_prng.seed ^= g_prng.seed << 5;

	return g_prng.seed;
}

/**
 * Generate random bytes
 */
void ws_random_bytes(uint8_t *buf, size_t len) {
	uint32_t r = 0;
	size_t i;

	for (i = 0; i < len; i++) {
		if ((i % 4) == 0) {
			r = prng_next();
		}
		buf[i] = (r >> ((i % 4) * 8)) & 0xFF;
	}
}
