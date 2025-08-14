/**
 * @file
 * @brief AT Device Emulator Implementation
 * 
 * @date July 08, 2025
 * @author Peize Li
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <at/emulator/at_emulator.h>
#include <at/at_parser.h>

#define INITIAL_CMD_CAPACITY 32

/* Helper functions */
static char *str_trim(char *str) {
	char *end;

	while (isspace((unsigned char)*str)) {
		str++;
	}

	if (*str == 0) {
		return str;
	}

	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) {
		end--;
	}

	end[1] = '\0';

	return str;
}

static int str_startswith_nocase(const char *str, const char *prefix) {
	while (*prefix) {
		if (toupper(*str) != toupper(*prefix)) {
			return 0;
		}
		str++;
		prefix++;
	}
	return 1;
}

/* Basic command handlers */
static at_result_t cmd_at(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	(void)emu;
	(void)type;
	(void)params;
	(void)response;
	(void)resp_size;
	return AT_OK;
}

static at_result_t cmd_ate(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	(void)type;
	(void)response;
	(void)resp_size;

	if (params && *params) {
		if (*params == '0') {
			at_emulator_set_echo(emu, false);
			return AT_OK;
		}
		else if (*params == '1') {
			at_emulator_set_echo(emu, true);
			return AT_OK;
		}
	}
	return AT_ERROR;
}

static at_result_t cmd_ati(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	(void)type;
	(void)params;

	snprintf(response, resp_size,
	    "%s\r\n"
	    "Manufacturer: %s\r\n"
	    "Model: %s\r\n"
	    "Version: %s",
	    emu->device_name, emu->manufacturer, emu->model, emu->version);
	return AT_OK;
}

static at_result_t cmd_atv(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	(void)type;
	(void)response;
	(void)resp_size;

	if (params && *params) {
		if (*params == '0') {
			at_emulator_set_verbose(emu, false);
			return AT_OK;
		}
		else if (*params == '1') {
			at_emulator_set_verbose(emu, true);
			return AT_OK;
		}
	}
	return AT_ERROR;
}

static at_result_t cmd_atq(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	(void)type;
	(void)response;
	(void)resp_size;

	if (params && *params) {
		if (*params == '0') {
			at_emulator_set_quiet(emu, false);
			return AT_OK;
		}
		else if (*params == '1') {
			at_emulator_set_quiet(emu, true);
			return AT_OK;
		}
	}
	return AT_ERROR;
}

static at_result_t cmd_atz(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	(void)type;
	(void)params;
	(void)response;
	(void)resp_size;

	/* Reset to defaults */
	at_emulator_set_echo(emu, true);
	at_emulator_set_verbose(emu, true);
	at_emulator_set_quiet(emu, false);

	return AT_OK;
}

static at_result_t cmd_gmr(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	(void)type;
	(void)params;

	if (type == AT_CMD_TEST) {
		snprintf(response, resp_size, "+GMR: (version info)");
		return AT_OK;
	}

	snprintf(response, resp_size, "%s", emu->version);
	return AT_OK;
}

static at_result_t cmd_gmm(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	(void)type;
	(void)params;

	if (type == AT_CMD_TEST) {
		snprintf(response, resp_size, "+GMM: (model info)");
		return AT_OK;
	}

	snprintf(response, resp_size, "%s", emu->model);
	return AT_OK;
}

static at_result_t cmd_gmi(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	(void)type;
	(void)params;

	if (type == AT_CMD_TEST) {
		snprintf(response, resp_size, "+GMI: (manufacturer info)");
		return AT_OK;
	}

	snprintf(response, resp_size, "%s", emu->manufacturer);
	return AT_OK;
}

at_emulator_t *at_emulator_create(void) {
	at_emulator_t *emu = calloc(1, sizeof(at_emulator_t));
	if (!emu) {
		return NULL;
	}

	at_emulator_set_device_info(emu, "Generic Device", "Embox", "AT-EMU", "1.0.0");

	emu->echo_enabled = true;
	emu->verbose_mode = true;
	emu->quiet_mode = false;

	emu->max_commands = INITIAL_CMD_CAPACITY;
	emu->commands = calloc(emu->max_commands, sizeof(at_command_t));
	if (!emu->commands) {
		free(emu);
		return NULL;
	}

	/* Initialize URC management */
	mutex_init(&emu->urc_mutex);
	emu->has_pending_urc = false;
	emu->device_poll = NULL;

	at_emulator_register_basic_commands(emu);

	return emu;
}

void at_emulator_destroy(at_emulator_t *emu) {
	if (emu) {
		free(emu->commands);
		free(emu);
	}
}

void at_emulator_set_device_info(at_emulator_t *emu, const char *name,
    const char *manufacturer, const char *model, const char *version) {
	if (!emu) {
		return;
	}

	if (name) {
		strncpy(emu->device_name, name, sizeof(emu->device_name) - 1);
	}
	if (manufacturer) {
		strncpy(emu->manufacturer, manufacturer, sizeof(emu->manufacturer) - 1);
	}
	if (model) {
		strncpy(emu->model, model, sizeof(emu->model) - 1);
	}
	if (version) {
		strncpy(emu->version, version, sizeof(emu->version) - 1);
	}
}

int at_emulator_register_command(at_emulator_t *emu, const char *name,
    at_cmd_handler_t handler) {
	if (!emu || !name || !handler) {
		return -1;
	}

	/* Check if command already exists */
	for (size_t i = 0; i < emu->num_commands; i++) {
		if (strcasecmp(emu->commands[i].name, name) == 0) {
			emu->commands[i].handler = handler;
			return 0;
		}
	}

	/* Expand array if needed */
	if (emu->num_commands >= emu->max_commands) {
		size_t new_capacity = emu->max_commands * 2;
		at_command_t *new_commands = realloc(emu->commands,
		    new_capacity * sizeof(at_command_t));
		if (!new_commands) {
			return -1;
		}
		emu->commands = new_commands;
		emu->max_commands = new_capacity;
	}

	emu->commands[emu->num_commands].name = name;
	emu->commands[emu->num_commands].handler = handler;
	emu->num_commands++;

	return 0;
}

void at_emulator_register_basic_commands(at_emulator_t *emu) {
	at_emulator_register_command(emu, "AT", cmd_at);
	at_emulator_register_command(emu, "ATE", cmd_ate);
	at_emulator_register_command(emu, "ATI", cmd_ati);
	at_emulator_register_command(emu, "ATV", cmd_atv);
	at_emulator_register_command(emu, "ATQ", cmd_atq);
	at_emulator_register_command(emu, "ATZ", cmd_atz);
	at_emulator_register_command(emu, "AT+GMR", cmd_gmr);
	at_emulator_register_command(emu, "AT+GMM", cmd_gmm);
	at_emulator_register_command(emu, "AT+GMI", cmd_gmi);
}

/* Find matching command and extract parameters */
static at_command_t *find_command(at_emulator_t *emu, const char *cmd_str,
    at_cmd_type_t *cmd_type, const char **params_out) {
	char cmd_name[64];
	at_cmd_type_t type;
	char params_buf[128];

	if (at_parse_cmd_type(cmd_str, cmd_name, sizeof(cmd_name), &type,
	        params_buf, sizeof(params_buf))
	    == 0) {
		*cmd_type = type;

		for (size_t i = 0; i < emu->num_commands; i++) {
			if (strcasecmp(emu->commands[i].name, cmd_name) == 0) {
				static char static_params[128];
				strcpy(static_params, params_buf);
				*params_out = static_params;
				return &emu->commands[i];
			}
		}
	}

	/* Fallback for simple commands like ATE0, ATV1 */
	for (size_t i = 0; i < emu->num_commands; i++) {
		if (str_startswith_nocase(cmd_str, emu->commands[i].name)) {
			size_t name_len = strlen(emu->commands[i].name);
			if (cmd_str[name_len] == '\0' || isdigit(cmd_str[name_len])) {
				*cmd_type = AT_CMD_SET;
				*params_out = cmd_str + name_len;
				return &emu->commands[i];
			}
		}
	}

	return NULL;
}

static void build_response(at_emulator_t *emu, at_result_t result,
    const char *data) {
	emu->resp_len = 0;

	if (emu->quiet_mode) {
		return;
	}

	int len = snprintf(emu->resp_buffer, sizeof(emu->resp_buffer), "\r\n");
	if (len > 0) {
		emu->resp_len += len;
	}

	if (data && *data) {
		len = snprintf(emu->resp_buffer + emu->resp_len,
		    sizeof(emu->resp_buffer) - emu->resp_len, "%s\r\n", data);
		if (len > 0) {
			emu->resp_len += len;
		}
	}

	const char *result_str = NULL;
	const char *numeric_str = NULL;

	switch (result) {
	case AT_OK:
		result_str = "OK";
		numeric_str = "0";
		break;
	case AT_ERROR:
		result_str = "ERROR";
		numeric_str = "4";
		break;
	case AT_BUSY:
		result_str = "BUSY";
		numeric_str = "7";
		break;
	case AT_NO_CARRIER:
		result_str = "NO CARRIER";
		numeric_str = "3";
		break;
	case AT_NO_DIALTONE:
		result_str = "NO DIALTONE";
		numeric_str = "6";
		break;
	case AT_NO_ANSWER:
		result_str = "NO ANSWER";
		numeric_str = "8";
		break;
	default:
		result_str = "ERROR";
		numeric_str = "4";
		break;
	}

	if (emu->verbose_mode) {
		len = snprintf(emu->resp_buffer + emu->resp_len,
		    sizeof(emu->resp_buffer) - emu->resp_len, "%s\r\n", result_str);
		if (len > 0) {
			emu->resp_len += len;
		}
	}
	else {
		len = snprintf(emu->resp_buffer + emu->resp_len,
		    sizeof(emu->resp_buffer) - emu->resp_len, "%s\r\n", numeric_str);
		if (len > 0) {
			emu->resp_len += len;
		}
	}
}

const char *at_emulator_process(at_emulator_t *emu, const char *input, size_t len) {
	static char echo_buf[2] = {0};

	if (!emu || !input || len == 0) {
		return "";
	}

	emu->resp_buffer[0] = '\0';
	emu->resp_len = 0;

	for (size_t i = 0; i < len; i++) {
		char ch = input[i];

		/* Handle backspace */
		if (ch == '\b' || ch == 0x7F) {
			if (emu->cmd_len > 0) {
				emu->cmd_len--;
				if (emu->echo_enabled) {
					strncpy(emu->resp_buffer, "\b \b", sizeof(emu->resp_buffer));
					emu->resp_len = 3;
					return emu->resp_buffer;
				}
			}
			continue;
		}

		/* Echo character if enabled (except for command completion) */
		/* Character would echoed immediately while processing, they wont in the response */
		if (emu->echo_enabled && ch != '\n') {
			echo_buf[0] = ch;
			echo_buf[1] = '\0';
			strncpy(emu->resp_buffer, echo_buf, sizeof(emu->resp_buffer));
			emu->resp_len = 1;
		}

		/* Check for line ending */
		if (ch == '\r') {
			if (emu->cmd_len > 0) {
				emu->cmd_buffer[emu->cmd_len] = '\0';

				char *cmd_str = str_trim(emu->cmd_buffer);

				if (!str_startswith_nocase(cmd_str, "AT")) {
					build_response(emu, AT_ERROR, NULL);
				}
				else {
					at_cmd_type_t cmd_type;
					const char *params;
					at_command_t *cmd = find_command(emu, cmd_str, &cmd_type,
					    &params);

					if (cmd) {
						char response_data[512] = {0};
						at_result_t result = cmd->handler(emu, cmd_type, params,
						    response_data, sizeof(response_data));
						build_response(emu, result, response_data);
					}
					else {
						build_response(emu, AT_ERROR, NULL);
					}
				}

				emu->cmd_len = 0;

				return emu->resp_buffer;
			}

			if (emu->echo_enabled) {
				return emu->resp_buffer;
			}
		}
		else if (ch == '\n') {
			/* Ignore LF - process on CR */
			if (emu->echo_enabled) {
				return emu->resp_buffer;
			}
			continue;
		}
		else {
			if (emu->cmd_len < sizeof(emu->cmd_buffer) - 1) {
				emu->cmd_buffer[emu->cmd_len++] = ch;
			}

			if (emu->echo_enabled) {
				return emu->resp_buffer;
			}
		}
	}

	return "";
}

const char *at_emulator_send_cmd(at_emulator_t *emu, const char *cmd) {
	const char *response = "";
	size_t i;

	for (i = 0; i < strlen(cmd); i++) {
		at_emulator_process(emu, &cmd[i], 1);
	}

	/* Send CR + LF to execute and get the final response */
	response = at_emulator_process(emu, "\r\n", 2);
	return response;
}

void at_emulator_set_echo(at_emulator_t *emu, bool enabled) {
	if (emu) {
		emu->echo_enabled = enabled;
	}
}

void at_emulator_set_verbose(at_emulator_t *emu, bool enabled) {
	if (emu) {
		emu->verbose_mode = enabled;
	}
}

void at_emulator_set_quiet(at_emulator_t *emu, bool enabled) {
	if (emu) {
		emu->quiet_mode = enabled;
	}
}

void at_emulator_set_user_data(at_emulator_t *emu, void *data) {
	if (emu) {
		emu->user_data = data;
	}
}

void *at_emulator_get_user_data(at_emulator_t *emu) {
	return emu ? emu->user_data : NULL;
}

/* Push URC message to emulator */
void at_emulator_push_urc(at_emulator_t *emu, const char *urc) {
	if (!emu || !urc) {
		return;
	}
	
	mutex_lock(&emu->urc_mutex);
	
	/* Simple implementation: overwrite any existing URC */
	strncpy(emu->pending_urc, urc, sizeof(emu->pending_urc) - 1);
	emu->pending_urc[sizeof(emu->pending_urc) - 1] = '\0';
	emu->has_pending_urc = true;
	
	mutex_unlock(&emu->urc_mutex);
}

/* Poll for pending URC messages */
const char *at_emulator_poll(at_emulator_t *emu) {
	static char urc_buffer[256];
	
	if (!emu) {
		return NULL;
	}
	
	/* First check device-specific polling */
	if (emu->device_poll) {
		const char *device_urc = emu->device_poll(emu);
		if (device_urc) {
			return device_urc;
		}
	}
	
	/* Then check generic URC queue */
	mutex_lock(&emu->urc_mutex);
	if (emu->has_pending_urc) {
		strcpy(urc_buffer, emu->pending_urc);
		emu->has_pending_urc = false;
		mutex_unlock(&emu->urc_mutex);
		return urc_buffer;
	}
	mutex_unlock(&emu->urc_mutex);
	
	return NULL;
}
