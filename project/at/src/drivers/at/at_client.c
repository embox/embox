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
#include <kernel/time/time.h>

int at_client_init_fd(at_client_t *client, int fd);

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
	client->owns_fd = false;  /* We don't own this fd */

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

	if (client->fd >= 0) {
		/* Only close fd if we opened it ourselves */
		if (client->owns_fd) {
			close(client->fd);
		}
		client->fd = -1;
	}

	/* Clear the structure */
	memset(client, 0, sizeof(*client));
	
	/* Set fd to invalid value */
	client->fd = -1;
}

static void process_line(at_client_t *client, const char *line) {
	at_resp_type_t type;

	/* Ignore empty lines */
	if (!line[0]) {
		return;
	}

	type = at_get_response_type(line);

	/* If waiting for response */
	if (client->waiting_resp) {
		/* Save response data */
		if (client->resp_buf && client->resp_size > 0
		    && (type == AT_RESP_DATA || type == AT_RESP_UNKNOWN)) {
			size_t len = strlen(line);
			size_t current_len = strlen(client->resp_buf);

			if (current_len + len + 3 < client->resp_size) {
				if (current_len > 0) {
					strcat(client->resp_buf, "\r\n");
				}
				strcat(client->resp_buf, line);
			}
		}

		/* Check if response complete */
		switch (type) {
		case AT_RESP_OK:
			client->result = AT_OK;
			client->waiting_resp = false;
			break;

		case AT_RESP_ERROR:
			client->result = AT_ERROR;
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
			/* Check for numeric response (numeric mode) */
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
	else if (client->urc_handler && at_is_urc(line)) {
		client->urc_handler(line, client->urc_arg);
	}
}

void at_client_process_rx(at_client_t *client) {
	unsigned char ch;
	ssize_t n;

	if (!client || client->fd < 0) {
		return;
	}

	/* Read all available characters */
	while ((n = read(client->fd, &ch, 1)) > 0) {
		/* Handle line ending */
		if (ch == '\r' || ch == '\n') {
			if (client->rx_pos > 0) {
				client->rx_buf[client->rx_pos] = '\0';
				process_line(client, client->rx_buf);
				client->rx_pos = 0;
			}
		}
		/* Accumulate characters */
		else if (client->rx_pos < sizeof(client->rx_buf) - 1) {
			client->rx_buf[client->rx_pos++] = ch;
		}
		/* Buffer overflow, discard */
		else {
			client->rx_pos = 0;
		}
	}
}

at_result_t at_client_send(at_client_t *client, const char *cmd, char *resp_buf,
    size_t resp_size, uint32_t timeout_ms) {
	uint32_t start_time, elapsed;
	ssize_t written;
	size_t total_written = 0;
	size_t cmd_len;

	if (!client || !cmd || client->fd < 0) {
		return AT_INVALID_PARAM;
	}

	/* Prepare to receive response */
	client->waiting_resp = true;
	client->result = AT_TIMEOUT;
	client->resp_buf = resp_buf;
	client->resp_size = resp_size;

	if (resp_buf && resp_size > 0) {
		resp_buf[0] = '\0';
	}

	/* Send command */
	cmd_len = strlen(cmd);
	while (total_written < cmd_len) {
		written = write(client->fd, cmd + total_written, cmd_len - total_written);
		if (written < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				/* Non-blocking write, retry */
				usleep(1000);
				continue;
			}
			client->waiting_resp = false;
			return AT_ERROR;
		}
		total_written += written;
	}

	/* Send \r\n */
	const char *crlf = "\r\n";
	total_written = 0;
	while (total_written < 2) {
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

	/* Wait for response */
	start_time = clock();
	while (client->waiting_resp) {
		at_client_process_rx(client);

		/* Check timeout */
		elapsed = clock() - start_time;
		if (elapsed > (timeout_ms * CLOCKS_PER_SEC / 1000)) {
			client->waiting_resp = false;
			return AT_TIMEOUT;
		}

		usleep(1000); /* 1ms */
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