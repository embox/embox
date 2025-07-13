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
#include <drivers/serial/uart_dev.h>
#include <kernel/time/time.h>

/* Initialize AT client - UART mode */
int at_client_init(at_client_t *client, const char *uart_name, int baudrate) {
	struct uart_params params;

	if (!client || !uart_name) {
		return -1;
	}

	memset(client, 0, sizeof(*client));
	client->fd = -1; /* Mark as UART mode */

	/* Set UART parameters */
	const char *dev_name = uart_name;
	if (strncmp(uart_name, "/dev/", 5) == 0) {
		dev_name = uart_name + 5;
	}

	client->uart = uart_dev_lookup(dev_name);
	if (!client->uart) {
		return -1;
	}

	/* Open UART if not already open */
	if (!(client->uart->state & UART_STATE_OPEN)) {
		if (uart_open(client->uart) < 0) {
			client->uart = NULL;
			return -1;
		}
	}

	if (uart_set_params(client->uart, &params) < 0) {
		uart_close(client->uart);
		client->uart = NULL;
		return -1;
	}

	return 0;
}

/* Initialize AT client - file descriptor mode */
int at_client_init_fd(at_client_t *client, int fd) {
	if (!client || fd < 0) {
		return -1;
	}

	memset(client, 0, sizeof(*client));
	client->fd = fd;
	client->uart = NULL; /* Mark as fd mode */

	/* Set non-blocking mode */
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags >= 0) {
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	}

	if (isatty(fd)) {
		struct termios tty;
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
		}
	}

	return 0;
}

void at_client_close(at_client_t *client) {
	if (client) {
		if (client->fd >= 0) {
			/* fd mode - don't close fd, managed by caller */
			client->fd = -1;
		}
		else if (client->uart) {
			/* UART mode - don't close UART, may be used by other components */
			client->uart = NULL;
		}
		memset(client, 0, sizeof(*client));
	}
}

/* Read one character from interface */
static int client_getc(at_client_t *client) {
	if (client->fd >= 0) {
		/* fd mode */
		unsigned char ch;
		int n = read(client->fd, &ch, 1);
		return (n == 1) ? ch : -1;
	}
	else if (client->uart) {
		/* UART mode */
		return uart_getc(client->uart);
	}
	return -1;
}

/* Write one character to interface */
static int client_putc(at_client_t *client, char ch) {
	if (client->fd >= 0) {
		/* fd mode */
		return (write(client->fd, &ch, 1) == 1) ? 0 : -1;
	}
	else if (client->uart) {
		/* UART mode */
		return uart_putc(client->uart, ch);
	}
	return -1;
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
	int ch;

	if (!client) {
		return;
	}

	/* Read all available characters */
	while ((ch = client_getc(client)) >= 0) {
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
			client->rx_buf[client->rx_pos++] = (char)ch;
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
	size_t i, len;

	if (!client || !cmd) {
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
	len = strlen(cmd);
	for (i = 0; i < len; i++) {
		if (client_putc(client, cmd[i]) < 0) {
			client->waiting_resp = false;
			return AT_ERROR;
		}
	}

	/* Send \r\n */
	client_putc(client, '\r');
	client_putc(client, '\n');

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