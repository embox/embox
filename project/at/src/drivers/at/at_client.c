/**
 * @file
 * @brief AT Command Client Implementation
 *
 * @date July 10, 2025
 * @author Peize Li
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <at/at_client.h>
#include <at/at_parser.h>
#include <hal/clock.h>
#include <kernel/time/time.h>

int at_client_init_fd(at_client_t *client, int fd);

#define AT_DEBUG(client, fmt, ...)                         \
	do {                                                   \
		if ((client)->flags & AT_CLIENT_FLAG_DEBUG) {      \
			printf("[AT_DEBUG] " fmt "\n", ##__VA_ARGS__); \
		}                                                  \
	} while (0)

/* Get current time in milliseconds */
static uint32_t get_current_ms(void) {
	return jiffies2ms(clock_sys_ticks());
}

/* Check if timeout occurred */
static bool is_timeout(uint32_t start_ms, uint32_t timeout_ms) {
	uint32_t current = get_current_ms();
	/* Handle wraparound */
	return (current - start_ms) >= timeout_ms;
}

/* Initialize AT client with device path */
int at_client_init(at_client_t *client, const char *device_path) {
	int fd;

	if (!client || !device_path) {
		return -1;
	}

	/* Open device */
	fd = open(device_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0) {
		return -1;
	}

	/* Use common initialization */
	if (at_client_init_fd(client, fd) < 0) {
		close(fd);
		return -1;
	}

	/* Mark that we own this fd */
	client->owns_fd = true;

	return 0;
}

/* Initialize AT client - file descriptor mode */
int at_client_init_fd(at_client_t *client, int fd) {
	struct termios tty;

	if (!client || fd < 0) {
		return -1;
	}

	memset(client, 0, sizeof(*client));
	client->fd = fd;
	client->owns_fd = false; /* We don't own this fd */
	client->mode = AT_CLIENT_MODE_TEST;
	client->flags = 0;

	/* Set non-blocking mode */
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags >= 0) {
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	}

	if (isatty(fd)) {
		if (tcgetattr(fd, &tty) == 0) {
			/* Clear character conversion in input flags */
			tty.c_iflag &= ~(ICRNL | IGNCR | INLCR);

			/* Clear character conversion in output flags */
			tty.c_oflag &= ~(ONLCR | OCRNL | ONLRET);

			/* Set to non-canonical mode */
			tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

			/* Set VMIN and VTIME */
			tty.c_cc[VMIN] = 0;  /* Non-blocking read */
			tty.c_cc[VTIME] = 0; /* No timeout */

			tcsetattr(fd, TCSANOW, &tty);
			tcflush(fd, TCIOFLUSH);
		}
	}

	return 0;
}

void at_client_close(at_client_t *client) {
	if (!client) {
		return;
	}

	/* Prevent double close */
	if (client->fd < 0) {
		return; // already closed
	}

	if (client->owns_fd) {
		close(client->fd);
	}

	/* clean up struct but keep fd invalid */
	memset(client, 0, sizeof(*client));
	client->fd = -1;
	client->mode = AT_CLIENT_MODE_TEST;
}

static void process_line(at_client_t *client, const char *line) {
	at_resp_type_t type;

	AT_DEBUG(client, "process_line: '%s'", line);

	/* Ignore empty lines */
	if (!line[0]) {
		return;
	}

	type = at_get_response_type(line);
	AT_DEBUG(client, "line type: %d", type);

	/* If waiting for response */
	if (client->waiting_resp) {
		if (client->resp_buf && client->resp_size > 0
		    && (type == AT_RESP_DATA || type == AT_RESP_UNKNOWN)) {
			/* Append new line before the response */
			size_t line_len = strlen(line);
			size_t current_len = strlen(client->resp_buf);

			AT_DEBUG(client, "appending to resp_buf, current_len=%zu", current_len);

			/* Calculate needed size: current + separator + new line + terminator */
			size_t separator_len = (current_len > 0) ? 2 : 0; /* "\r\n" */
			size_t needed = current_len + separator_len + line_len + 1;

			if (needed <= client->resp_size) {
				if (current_len > 0) {
					strcat(client->resp_buf, "\r\n");
				}
				strcat(client->resp_buf, line);
			}
			else {
				/* buffer overflow, truncate response */
				size_t available = client->resp_size - current_len
				                   - separator_len - 1;
				if (available > 0
				    && current_len + separator_len < client->resp_size) {
					if (current_len > 0) {
						strcat(client->resp_buf, "\r\n");
					}
					strncat(client->resp_buf, line, available);
					/* Ensure null-termination */
					client->resp_buf[client->resp_size - 1] = '\0';
				}
				/* Set truncated flag */
				client->flags |= AT_CLIENT_FLAG_RESP_TRUNCATED;
			}
		}

		/* Check if response complete */
		switch (type) {
		case AT_RESP_OK:
			AT_DEBUG(client, "Got OK, ending response");
			client->result = AT_OK;
			client->waiting_resp = false;
			break;

		case AT_RESP_ERROR:
			/* Extract CME/CMS error code */
			if (strncmp(line, "+CME ERROR:", 11) == 0) {
				client->result = AT_CME_ERROR;
			}
			else if (strncmp(line, "+CMS ERROR:", 11) == 0) {
				client->result = AT_CMS_ERROR;
			}
			else {
				client->result = AT_ERROR;
			}
			client->waiting_resp = false;
			break;

		case AT_RESP_SEND_OK:
			client->result = AT_OK;
			client->waiting_resp = false;
			break;

		case AT_RESP_SEND_FAIL:
			client->result = AT_SEND_FAIL;
			client->waiting_resp = false;
			break;

		default:
			/* Check for numeric response (verbose mode disabled) */
			if (strlen(line) == 1 && line[0] >= '0' && line[0] <= '9') {
				switch (line[0]) {
				case '0': /* OK */
					client->result = AT_OK;
					client->waiting_resp = false;
					break;
				case '3': /* NO CARRIER */
					client->result = AT_NO_CARRIER;
					client->waiting_resp = false;
					break;
				case '4': /* ERROR */
					client->result = AT_ERROR;
					client->waiting_resp = false;
					break;
				case '6': /* NO DIALTONE */
					client->result = AT_NO_DIALTONE;
					client->waiting_resp = false;
					break;
				case '7': /* BUSY */
					client->result = AT_BUSY;
					client->waiting_resp = false;
					break;
				case '8': /* NO ANSWER */
					client->result = AT_NO_ANSWER;
					client->waiting_resp = false;
					break;
				default:
					break;
				}
			}
			break;
		}
	}
	/* Otherwise might be URC */
	else if (client->urc_handler) {
		/* Call URC handler */
		client->urc_handler(line, client->urc_arg);
	}
}

void at_client_process_rx(at_client_t *client) {
	unsigned char buffer[256]; /* Buffer for reading data */
	ssize_t n;
	int i;
	int consecutive_empty = 0;
	const int max_empty_reads = (client->mode == AT_CLIENT_MODE_NORMAL) ? 3 : 1;
	const int read_delay_us = (client->mode == AT_CLIENT_MODE_NORMAL) ? 5000 : 0;
	const int empty_delay_us = (client->mode == AT_CLIENT_MODE_NORMAL) ? 10000 : 0;

	if (!client || client->fd < 0) {
		return;
	}

	/* Continuous reading until several empty reads */
	while (consecutive_empty < max_empty_reads) {
		n = read(client->fd, buffer, sizeof(buffer));

		if (n > 0) {
			consecutive_empty = 0;
			AT_DEBUG(client, "Received %zd bytes", n);

			for (i = 0; i < n; i++) {
				unsigned char ch = buffer[i];

				if (ch == '\r' || ch == '\n') {
					if (client->rx_pos > 0) {
						client->rx_buf[client->rx_pos] = '\0';
						AT_DEBUG(client, "Complete line: '%s'", client->rx_buf);
						process_line(client, client->rx_buf);
						client->rx_pos = 0;
					}
				}
				else if (client->rx_pos < AT_CLIENT_RX_BUFFER_SIZE - 1) {
					client->rx_buf[client->rx_pos++] = ch;
				}
				else {
					/* Buffer overflow - process current line */
					AT_DEBUG(client, "RX buffer overflow!");
					client->rx_buf[client->rx_pos] = '\0';
					process_line(client, client->rx_buf);
					client->rx_pos = 0;
					client->rx_buf[client->rx_pos++] = ch;
				}
			}

			if (read_delay_us > 0) {
				usleep(read_delay_us);
			}
		}
		else if (n == 0 || (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))) {
			consecutive_empty++;
			if (consecutive_empty < max_empty_reads && empty_delay_us > 0) {
				usleep(empty_delay_us);
			}
		}
		else {
			AT_DEBUG(client, "Read error: %s", strerror(errno));
			break;
		}
	}

	/* Handle incomplete line for URC */
	if (client->rx_pos > 0 && !client->waiting_resp) {
		client->rx_buf[client->rx_pos] = '\0';
		if (client->urc_handler) {
			client->urc_handler(client->rx_buf, client->urc_arg);
		}
		client->rx_pos = 0;
	}
}

at_result_t at_client_send(at_client_t *client, const char *cmd, char *resp_buf,
    size_t resp_size, uint32_t timeout_ms) {
	if (!client || !cmd || client->fd < 0) {
		return AT_INVALID_PARAM;
	}

	uint32_t start_time;
	ssize_t written;
	size_t total_written = 0;
	size_t cmd_len;
	at_client_mode_t mode = client->mode;
	const bool is_normal_mode = (mode == AT_CLIENT_MODE_NORMAL);
	const int retry_count = is_normal_mode ? 3 : 1;
	const int retry_delay_us = is_normal_mode ? 2000 : 0;
	const int post_send_delay_us = is_normal_mode ? 10000 : 0;

	AT_DEBUG(client, "Sending: %s", cmd);

	/* Drain receive buffer in NORMAL mode */
	if (client->mode == AT_CLIENT_MODE_NORMAL) {
		int old_flags = fcntl(client->fd, F_GETFL, 0);
		fcntl(client->fd, F_SETFL, old_flags | O_NONBLOCK);

		char drain_buf[256];
		int drain_count = 0;
		while (read(client->fd, drain_buf, sizeof(drain_buf)) > 0) {
			drain_count++;
			if (drain_count > 10)
				break;
		}

		fcntl(client->fd, F_SETFL, old_flags);
	}

	/* Clear line buffer */
	client->rx_pos = 0;

	/* Prepare for response */
	client->waiting_resp = true;
	client->result = AT_TIMEOUT;
	client->resp_buf = resp_buf;
	client->resp_size = resp_size;
	client->flags &= ~AT_CLIENT_FLAG_RESP_TRUNCATED;

	if (resp_buf && resp_size > 0) {
		resp_buf[0] = '\0';
	}

	start_time = get_current_ms();

	/* Send command */
	cmd_len = strlen(cmd);
	while (total_written < cmd_len) {
		if (is_timeout(start_time, timeout_ms)) {
			client->waiting_resp = false;
			return AT_TIMEOUT;
		}

		written = write(client->fd, cmd + total_written, cmd_len - total_written);
		if (written < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				usleep(1000);
				continue;
			}
			client->waiting_resp = false;
			return AT_ERROR;
		}
		total_written += written;
	}

	/* Send CRLF */
	const char *crlf = "\r\n";
	total_written = 0;
	while (total_written < 2) {
		if (is_timeout(start_time, timeout_ms)) {
			client->waiting_resp = false;
			return AT_TIMEOUT;
		}

		written = write(client->fd, crlf + total_written, 2 - total_written);
		if (written < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				usleep(1000);
				continue;
			}
			client->waiting_resp = false;
			return AT_ERROR;
		}
		total_written += written;
	}

	if (post_send_delay_us > 0) {
		usleep(post_send_delay_us);
	}

	/* Wait for response */
	int no_progress_count = 0;
	size_t last_resp_len = 0;

	while (client->waiting_resp) {
		for (int i = 0; i < retry_count && client->waiting_resp; i++) {
			at_client_process_rx(client);
			if (!client->waiting_resp)
				break;
			if (retry_delay_us > 0) {
				usleep(retry_delay_us);
			}
		}

		/* Check progress */
		size_t current_resp_len = (resp_buf && resp_size > 0) ? strlen(resp_buf)
		                                                      : 0;
		if (current_resp_len > last_resp_len) {
			no_progress_count = 0;
			last_resp_len = current_resp_len;
		}
		else {
			no_progress_count++;
		}

		if (is_timeout(start_time, timeout_ms)) {
			client->waiting_resp = false;
			AT_DEBUG(client, "TIMEOUT after %u ms", timeout_ms);
			return AT_TIMEOUT;
		}

		usleep(5000);
	}

	AT_DEBUG(client, "Response complete: result=%d", client->result);
	if (resp_buf && resp_size > 0) {
		AT_DEBUG(client, "Response buffer has %zu chars", strlen(resp_buf));
	}

	return client->result;
}

void at_client_set_urc_handler(at_client_t *client,
    void (*handler)(const char *, void *), void *arg) {
	if (client) {
		client->urc_handler = handler;
		client->urc_arg = arg;
	}
}

void at_client_set_mode(at_client_t *client, at_client_mode_t mode) {
	if (client) {
		client->mode = mode;
	}
}

void at_client_enable_debug(at_client_t *client, bool enable) {
	if (client) {
		if (enable) {
			client->flags |= AT_CLIENT_FLAG_DEBUG;
		}
		else {
			client->flags &= ~AT_CLIENT_FLAG_DEBUG;
		}
	}
}
