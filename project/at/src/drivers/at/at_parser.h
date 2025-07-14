/**
 * @file
 * @brief AT Command Parser Header
 *
 * @date July 05, 2025
 * @author Peize Li
 */

#ifndef AT_PARSER_H
#define AT_PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief AT command types
 */
typedef enum {
	AT_CMD_EXEC, /**< Execute command: AT+CMD */
	AT_CMD_SET,  /**< Set command: AT+CMD=value */
	AT_CMD_READ, /**< Read command: AT+CMD? */
	AT_CMD_TEST  /**< Test command: AT+CMD=? */
} at_cmd_type_t;

/**
 * @brief AT response types
 */
typedef enum {
	AT_RESP_OK,        /**< OK response */
	AT_RESP_ERROR,     /**< ERROR response */
	AT_RESP_DATA,      /**< Data response: +CMD:data */
	AT_RESP_PROMPT,    /**< Prompt: > */
	AT_RESP_SEND_OK,   /**< SEND OK */
	AT_RESP_SEND_FAIL, /**< SEND FAIL */
	AT_RESP_UNKNOWN    /**< Unknown response */
} at_resp_type_t;

/**
 * @brief AT operation result codes
 */
typedef enum {
	AT_OK = 0,              /**< Success */
	AT_ERROR = -1,          /**< General error */
	AT_TIMEOUT = -2,        /**< Timeout */
	AT_BUSY = -3,           /**< Device busy */
	AT_NO_CARRIER = -4,     /**< No carrier */
	AT_NO_DIALTONE = -5,    /**< No dial tone */
	AT_NO_ANSWER = -6,      /**< No answer */
	AT_CME_ERROR = -7,      /**< CME error */
	AT_CMS_ERROR = -8,      /**< CMS error */
	AT_SEND_FAIL = -9,      /**< Send failed */
	AT_INVALID_PARAM = -10, /**< Invalid parameter */
	AT_NOT_SUPPORTED = -11  /**< Not supported */
} at_result_t;

/**
 * @brief Parsed AT response
 */
typedef struct {
	at_resp_type_t type; /**< Response type */
	char cmd[32];        /**< Command name, e.g. "+CWMODE" */
	char data[256];      /**< Response data */
	int error_code;      /**< Error code (CME/CMS ERROR) */
} at_response_t;

/**
 * @brief AT transport interface
 * 
 * Abstract transport layer interface (UART, SPI, USB, etc.)
 */
struct at_transport {
	/**
     * @brief Send data
     * @param transport Transport interface pointer
     * @param data Data to send
     * @param len Data length
     * @return Number of bytes sent, negative on error
     */
	int (*send)(struct at_transport *transport, const void *data, size_t len);

	/**
     * @brief Receive data
     * @param transport Transport interface pointer
     * @param data Receive buffer
     * @param len Buffer size
     * @param timeout_ms Timeout in milliseconds
     * @return Number of bytes received, negative on error
     */
	int (*recv)(struct at_transport *transport, void *data, size_t len,
	    uint32_t timeout_ms);

	void *priv; /**< Private data */
};

/**
 * @brief URC (Unsolicited Result Code) callback function type
 * @param line Received URC line
 * @param user_data User data
 */
typedef void (*at_urc_callback_t)(const char *line, void *user_data);

/**
 * @brief Parse AT command type
 * 
 * Parse command string to extract name, type and parameters
 * 
 * @param cmd Command string
 * @param name Buffer for command name
 * @param name_size Name buffer size
 * @param type Output command type
 * @param params Buffer for parameters (optional)
 * @param param_size Parameter buffer size
 * @return 0 on success, -1 on failure
 */
int at_parse_cmd_type(const char *cmd, char *name, size_t name_size,
    at_cmd_type_t *type, char *params, size_t param_size);

/**
 * @brief Get response type
 * 
 * @param line Response line
 * @return Response type
 */
at_resp_type_t at_get_response_type(const char *line);

/**
 * @brief Parse AT response
 * 
 * Extract type, command, data from response line
 * 
 * @param response Response string
 * @param result Parsed result output
 * @return 0 on success, -1 on failure
 */
int at_parse_response(const char *response, at_response_t *result);

/**
 * @brief Check if line is URC
 * 
 * @param line Line to check
 * @return true if URC, false otherwise
 */
bool at_is_urc(const char *line);

/**
 * @brief Parse +IPD packet format
 * 
 * Parse ESP8266/ESP32 +IPD format:
 * +IPD,<link_id>,<length>:data or +IPD,<length>:data
 * 
 * @param line IPD line
 * @param link_id Output connection ID
 * @param length Output data length
 * @param data_start Output data start position
 * @return 0 on success, -1 on failure
 */
int at_parse_ipd(const char *line, int *link_id, size_t *length,
    const char **data_start);

/**
 * @brief Extract quoted string
 * 
 * Extract content between first pair of double quotes
 * 
 * @param input Input string
 * @param output Output buffer
 * @param out_size Output buffer size
 * @return 0 on success, -1 on failure
 */
int at_extract_quoted_string(const char *input, char *output, size_t out_size);

/**
 * @brief Parse comma-separated parameters
 * 
 * Split comma-separated parameter string into array.
 * Supports commas within quotes.
 * 
 * @param params Parameter string
 * @param argv Output parameter pointer array
 * @param max_args Maximum number of parameters
 * @return Number of parameters parsed
 */
int at_parse_params(const char *params, char *argv[], int max_args);

/**
 * @brief Parse CME ERROR
 * 
 * Parse +CME ERROR: format
 * 
 * @param line Error line
 * @param error_code Output error code
 * @return 0 on success, -1 on failure
 */
int at_parse_cme_error(const char *line, int *error_code);

/**
 * @brief Parse integer value
 * 
 * @param str String to parse
 * @param value Output integer value
 * @return 0 on success, -1 on failure
 */
int at_parse_integer(const char *str, int *value);

#endif /* AT_PARSER_H */