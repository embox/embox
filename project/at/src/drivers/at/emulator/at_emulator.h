/**
 * @file
 * @brief AT Device Emulator Header
 *
 * @date July 08, 2025
 * @author Peize Li
 */

#ifndef AT_EMULATOR_H
#define AT_EMULATOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <at/at_parser.h>

/* Forward declarations */
typedef struct at_emulator at_emulator_t;

/**
 * @brief Command handler function type
 * @param emu Emulator instance
 * @param type Command type (EXEC, SET, READ, TEST)
 * @param params Command parameters
 * @param response Buffer for response data
 * @param resp_size Response buffer size
 * @return AT result code
 */
typedef at_result_t (*at_cmd_handler_t)(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size);

/**
 * @brief Command definition
 */
typedef struct {
	const char *name;         /**< Command name (e.g., "AT+GMR") */
	at_cmd_handler_t handler; /**< Handler function */
} at_command_t;

/**
 * @brief AT Emulator structure
 */
struct at_emulator {
	/* Device information */
	char device_name[32];
	char manufacturer[32];
	char model[32];
	char version[32];

	/* Command buffer */
	char cmd_buffer[256];
	size_t cmd_len;

	/* Response buffer */
	char resp_buffer[1024];
	size_t resp_len;

	/* Configuration */
	bool echo_enabled;
	bool verbose_mode;
	bool quiet_mode;

	/* Registered commands */
	at_command_t *commands;
	size_t num_commands;
	size_t max_commands;

	/* User data pointer */
	void *user_data;
};

/**
 * @brief Create AT emulator instance
 * 
 * Creates and initializes a new AT command emulator with default settings.
 * Default settings: echo enabled, verbose mode, quiet disabled.
 * 
 * @return New emulator instance, NULL on allocation failure
 */
at_emulator_t *at_emulator_create(void);

/**
 * @brief Destroy AT emulator instance
 * 
 * @param emu Emulator instance to destroy
 */
void at_emulator_destroy(at_emulator_t *emu);

/**
 * @brief Set device information
 * 
 * Configure device identification strings returned by ATI and related commands.
 * 
 * @param emu Emulator instance
 * @param name Device name
 * @param manufacturer Manufacturer string
 * @param model Model string
 * @param version Version string
 */
void at_emulator_set_device_info(at_emulator_t *emu, const char *name,
    const char *manufacturer, const char *model, const char *version);

/**
 * @brief Register AT command handler
 * 
 * Register a custom command handler. If command already exists, 
 * the handler will be updated.
 * 
 * @param emu Emulator instance
 * @param name Command name (e.g., "AT+CUSTOM")
 * @param handler Command handler function
 * @return 0 on success, -1 on failure
 */
int at_emulator_register_command(at_emulator_t *emu, const char *name,
    at_cmd_handler_t handler);

/**
 * @brief Register basic AT commands
 * 
 * Registers standard AT commands: AT, ATE, ATI, ATV, ATQ, ATZ,
 * AT+GMR, AT+GMM, AT+GMI
 * 
 * @param emu Emulator instance
 */
void at_emulator_register_basic_commands(at_emulator_t *emu);

/**
 * @brief Process input data
 * 
 * Process input characters and generate appropriate responses.
 * Handles character echo, command parsing, and response generation.
 * 
 * @param emu Emulator instance
 * @param input Input data buffer
 * @param len Input data length
 * @return Response string (empty if no response ready)
 */
const char *at_emulator_process(at_emulator_t *emu, const char *input, size_t len);

/**
 * @brief Send command to emulator
 * 
 * Sends a command string to the emulator and waits for response.
 * 
 * @param emu Emulator instance
 * @param cmd Command string (e.g., "AT+GMR")
 * @return Response string, NULL if command failed or no response
 */
const char *at_emulator_send_cmd(at_emulator_t *emu, const char *cmd);

/**
 * @brief Set echo mode
 * 
 * Enable/disable character echo (ATE0/ATE1)
 * 
 * @param emu Emulator instance
 * @param enabled true to enable echo, false to disable
 */
void at_emulator_set_echo(at_emulator_t *emu, bool enabled);

/**
 * @brief Set verbose mode
 * 
 * Enable/disable verbose responses (ATV0/ATV1).
 * Verbose mode returns text responses (OK, ERROR).
 * Non-verbose mode returns numeric codes (0, 4).
 * 
 * @param emu Emulator instance
 * @param enabled true for verbose mode, false for numeric
 */
void at_emulator_set_verbose(at_emulator_t *emu, bool enabled);

/**
 * @brief Set quiet mode
 * 
 * Enable/disable response suppression (ATQ0/ATQ1).
 * Quiet mode suppresses all responses.
 * 
 * @param emu Emulator instance
 * @param enabled true to suppress responses, false for normal
 */
void at_emulator_set_quiet(at_emulator_t *emu, bool enabled);

/**
 * @brief Set user data
 * 
 * Associate user data with emulator instance for use in command handlers.
 * 
 * @param emu Emulator instance
 * @param data User data pointer
 */
void at_emulator_set_user_data(at_emulator_t *emu, void *data);

/**
 * @brief Get user data
 * 
 * @param emu Emulator instance
 * @return User data pointer, NULL if not set
 */
void *at_emulator_get_user_data(at_emulator_t *emu);

#endif /* AT_EMULATOR_H */