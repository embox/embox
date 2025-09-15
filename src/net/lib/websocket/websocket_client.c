/**
 * @file
 * @brief WebSocket Client Handshake
 * 
 * @date Sept 10, 2025
 * @author Peize Li
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <lib/crypt/b64.h>
#include <lib/crypt/sha1.h>
#include <net/lib/websocket.h>

static int ws_generate_accept_key(const char *client_key, char *accept_key) {
	char combined[256];
	uint8_t hash[SHA1_DIGEST_SIZE];
	size_t encoded_len;

	snprintf(combined, sizeof(combined), "%s%s", client_key, WS_GUID);
	sha1_count((const uint8_t *)combined, strlen(combined), hash);

	if (b64_encode((char *)hash, SHA1_DIGEST_SIZE, accept_key,
	        WS_ACCEPT_LEN + 1, &encoded_len)
	    != 0) {
		return -1;
	}

	accept_key[encoded_len] = '\0';
	return 0;
}

int ws_handshake(ws_connection_t *ws, const char *host, const char *path) {
	char request[1024];
	char response[4096];
	char key[64];
	char expected_accept[64];
	uint8_t key_bytes[WS_KEY_LEN];
	size_t key_len;

	/* Generate random key */
	ws_random_bytes(key_bytes, WS_KEY_LEN);

	if (b64_encode((char *)key_bytes, WS_KEY_LEN, key, sizeof(key), &key_len) != 0) {
		fprintf(stderr, "Failed to encode key\n");
		return -1;
	}
	key[key_len] = '\0';

	/* Build HTTP upgrade request */
	int req_len = snprintf(request, sizeof(request),
	    "GET %s HTTP/1.1\r\n"
	    "Host: %s\r\n"
	    "Upgrade: websocket\r\n"
	    "Connection: Upgrade\r\n"
	    "Sec-WebSocket-Key: %s\r\n"
	    "Sec-WebSocket-Version: 13\r\n"
	    "Origin: http://%s\r\n"
	    "\r\n",
	    path, host, key, host);

	printf("=== SENDING REQUEST (%d bytes) ===\n", req_len);
	printf("%s", request);
	printf("=== END REQUEST ===\n");

	/* Send request */
	int sent = send(ws->sock_fd, request, req_len, 0);
	if (sent != req_len) {
		fprintf(stderr, "Send failed: sent %d of %d bytes\n", sent, req_len);
		return -1;
	}

	/* Read response */
	memset(response, 0, sizeof(response));
	int total = 0;
	int attempts = 0;
	const int max_attempts = 20;

	while (attempts < max_attempts) {
		int n = recv(ws->sock_fd, response + total,
		    sizeof(response) - total - 1, 0);

		if (n > 0) {
			total += n;
			response[total] = '\0';

			printf("=== RECEIVED %d bytes (total: %d) ===\n", n, total);

			/* Check if complete HTTP headers received */
			if (strstr(response, "\r\n\r\n") != NULL) {
				printf("=== COMPLETE RESPONSE ===\n%s\n=== END RESPONSE ===\n",
				    response);
				break;
			}
		}
		else if (n == 0) {
			fprintf(stderr, "Connection closed by server\n");
			return -1;
		}
		else {
			/* Check if would block */
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				fprintf(stderr, "Recv error: %s\n", strerror(errno));
				return -1;
			}
		}

		attempts++;
		usleep(50000); /* 50ms delay */
	}

	if (attempts >= max_attempts) {
		fprintf(stderr, "Timeout waiting for complete response\n");
		fprintf(stderr, "Partial response (%d bytes):\n%s\n", total, response);
		return -1;
	}

	/* Parse status line */
	char *status_line = response;
	char *line_end = strstr(status_line, "\r\n");
	if (!line_end) {
		fprintf(stderr, "No status line found\n");
		return -1;
	}

	*line_end = '\0';
	printf("Status line: '%s'\n", status_line);
	*line_end = '\r';

	/* Check for 101 status code */
	if (!strstr(status_line, "101")) {
		fprintf(stderr, "Expected 101 status code, got: %s\n", status_line);
		return -1;
	}

	/* Find required headers */
	char *p = response;
	int found_upgrade = 0;
	int found_connection = 0;
	char *accept_value = NULL;

	while ((p = strstr(p, "\r\n")) != NULL) {
		p += 2;

		if (strncasecmp(p, "upgrade:", (size_t)8) == 0) {
			if (strcasestr(p, "websocket")) {
				found_upgrade = 1;
				printf("Found Upgrade header\n");
			}
		}
		else if (strncasecmp(p, "connection:", (size_t)11) == 0) {
			if (strcasestr(p, "upgrade")) {
				found_connection = 1;
				printf("Found Connection header\n");
			}
		}
		else if (strncasecmp(p, "sec-websocket-accept:", (size_t)21) == 0) {
			accept_value = p + 21;
			while (*accept_value == ' ')
				accept_value++;

			char *accept_end = strpbrk(accept_value, "\r\n");
			if (accept_end) {
				size_t len = accept_end - accept_value;
				char received[64];
				memcpy(received, accept_value, len);
				received[len] = '\0';

				ws_generate_accept_key(key, expected_accept);

				printf("Expected accept: '%s'\n", expected_accept);
				printf("Received accept: '%s'\n", received);

				if (strcmp(received, expected_accept) != 0) {
					fprintf(stderr, "Accept key mismatch!\n");
					return -1;
				}
				printf("Accept key verified\n");
			}
		}
	}

	if (!found_upgrade || !found_connection || !accept_value) {
		fprintf(stderr, "Missing required headers\n");
		fprintf(stderr, "upgrade=%d, connection=%d, accept=%p\n", found_upgrade,
		    found_connection, accept_value);
		return -1;
	}

	ws->state = WS_STATE_OPEN;
	if (ws->callbacks.on_open) {
		ws->callbacks.on_open(ws);
	}

	return 0;
}
