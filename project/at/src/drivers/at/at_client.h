/**
 * @file
 * @brief AT Command Client Header
 *
 * @date July 10, 2025
 * @author Peize Li
 */

#ifndef AT_CLIENT_H
#define AT_CLIENT_H

#include <stdbool.h>
#include <stdint.h>

#include <at/at_parser.h>

/**
 * @brief AT Client structure
 */
typedef struct at_client {
	/* Communication interface */
	int fd; /**< File descriptor */

	/* Flag indicating if we own the fd and should close it */
	bool owns_fd;

	/* Receive buffer */
	char rx_buf[256]; /**< Receive line buffer */
	size_t rx_pos;    /**< Current position */

	/* Current pending response */
	bool waiting_resp;  /**< Waiting for response flag */
	at_result_t result; /**< Response result */
	char *resp_buf;     /**< User-provided response buffer */
	size_t resp_size;   /**< Response buffer size */

	/* URC callback */
	void (*urc_handler)(const char *line, void *arg);
	void *urc_arg;
} at_client_t;

/**
 * @brief Initialize AT client with device path and baud rate
 * 
 * @param client Client instance
 * @param device_path Device path (e.g., "/dev/ttyS1")
 * @return 0 on success, -1 on error
 */
int at_client_init(at_client_t *client, const char *device_path);

/**
 * @brief Initialize AT client with file descriptor
 * 
 * @param client Client instance
 * @param fd File descriptor (e.g., PTY, socket)
 * @return 0 on success, -1 on failure
 */
int at_client_init_fd(at_client_t *client, int fd);

/**
 * @brief Close AT client
 * 
 * @param client Client instance
 */
void at_client_close(at_client_t *client);

/**
 * @brief Send AT command and wait for response
 * 
 * @param client Client instance
 * @param cmd AT command (without \r\n)
 * @param resp_buf Response buffer (optional)
 * @param resp_size Buffer size
 * @param timeout_ms Timeout in milliseconds
 * @return AT result code
 */
at_result_t at_client_send(at_client_t *client, const char *cmd, char *resp_buf,
    size_t resp_size, uint32_t timeout_ms);

/**
 * @brief Set URC handler
 * 
 * @param client Client instance
 * @param handler Handler function
 * @param arg User argument for handler
 */
void at_client_set_urc_handler(at_client_t *client,
    void (*handler)(const char *, void *), void *arg);

/**
 * @brief Process received data
 * 
 * Called by receive thread or main loop to process incoming data.
 * 
 * @param client Client instance
 */
void at_client_process_rx(at_client_t *client);

#endif /* AT_CLIENT_H */