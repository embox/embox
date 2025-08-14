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

/* AT Client receive buffer size */
#define AT_CLIENT_RX_BUFFER_SIZE 1024
#define AT_CLIENT_MIN_RESP_SIZE  512

/* AT Client flags */
#define AT_CLIENT_FLAG_DEBUG          0x01 /* Debug mode */
#define AT_CLIENT_FLAG_RESP_TRUNCATED 0x02 /* Response was truncated */

/* AT Client operation modes */
typedef enum {
	AT_CLIENT_MODE_TEST = 0,
	AT_CLIENT_MODE_NORMAL = 1,
} at_client_mode_t;

/**
 * @brief AT Client structure
 */
typedef struct at_client {
	/* Communication interface */
	int fd; /**< File descriptor */

	/* Flag indicating if we own the fd and should close it */
	bool owns_fd;

	/* Receive buffer */
	char rx_buf[AT_CLIENT_RX_BUFFER_SIZE]; /**< Receive line buffer */
	size_t rx_pos;                         /**< Current position */

	/* Current pending response */
	bool waiting_resp;  /**< Waiting for response flag */
	at_result_t result; /**< Response result */
	char *resp_buf;     /**< User-provided response buffer */
	size_t resp_size;   /**< Response buffer size */

	/* URC callback */
	void (*urc_handler)(const char *line, void *arg);
	void *urc_arg;

	/* Status flags */
	uint8_t flags; /**< Status flags (e.g., AT_CLIENT_FLAG_RESP_TRUNCATED) */

	/* Operation mode */
	at_client_mode_t mode; /**< Current operation mode */
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
 * @brief Send AT command and wait for response, resp_buf should be at least AT_CLIENT_MIN_RESP_SIZE bytes
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

/**
 * @brief Check if last response was truncated
 * 
 * @param client Client instance
 * @return true if response was truncated, false otherwise
 */
static inline bool at_client_response_truncated(at_client_t *client) {
	return client ? (client->flags & AT_CLIENT_FLAG_RESP_TRUNCATED) : false;
}

/**
 * @brief Set AT client operation mode
 * 
 * @param client Client instance
 * @param mode Operation mode (AT_CLIENT_MODE_TEST or AT_CLIENT_MODE_NORMAL)
 */
void at_client_set_mode(at_client_t *client, at_client_mode_t mode);

/**
 * @brief Enable or disable debug mode
 * 
 * @param client Client instance
 * @param enable true to enable debug mode, false to disable
 */
void at_client_enable_debug(at_client_t *client, bool enable);

#endif /* AT_CLIENT_H */