/**
 * @file
 * @brief AT Command Parser Implementation
 * 
 * @date July 05, 2025
 * @author Peize Li
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drivers/at/at_parser.h>

/* Parse AT command type */
int at_parse_cmd_type(const char *cmd, char *name, size_t name_size,
    at_cmd_type_t *type, char *params, size_t param_size) {
	const char *p = cmd;
	size_t name_len = 0;

	/* Skip leading whitespace */
	while (*p && isspace(*p)) {
		p++;
	}

	/* Find end of command name */
	const char *name_start = p;
	while (*p && *p != '=' && *p != '?' && !isspace(*p)) {
		name_len++;
		p++;
	}

	/* Copy command name */
	if (name_len == 0 || name_len >= name_size) {
		return -1;
	}
	strncpy(name, name_start, name_len);
	name[name_len] = '\0';

	if (params) {
		params[0] = '\0';
	}

	while (*p && isspace(*p)) {
		p++;
	}

	/* Determine command type */
	if (*p == '\0') {
		*type = AT_CMD_EXEC;
	}
	else if (*p == '?') {
		*type = AT_CMD_READ;
	}
	else if (*p == '=') {
		p++;
		if (*p == '?') {
			*type = AT_CMD_TEST;
		}
		else {
			*type = AT_CMD_SET;
			if (params && param_size > 0) {
				strncpy(params, p, param_size - 1);
				params[param_size - 1] = '\0';
			}
		}
	}
	else {
		return -1;
	}

	return 0;
}

/* Get response type */
at_resp_type_t at_get_response_type(const char *line) {
	while (*line && isspace(*line)) {
		line++;
	}

	if (strncmp(line, "OK", 2) == 0 && (line[2] == '\0' || isspace(line[2]))) {
		return AT_RESP_OK;
	}
	if (strncmp(line, "ERROR", 5) == 0) {
		return AT_RESP_ERROR;
	}
	if (strncmp(line, "+CME ERROR:", 11) == 0) {
		return AT_RESP_ERROR;
	}
	if (strncmp(line, "+CMS ERROR:", 11) == 0) {
		return AT_RESP_ERROR;
	}
	if (strcmp(line, ">") == 0 || strcmp(line, "> ") == 0) {
		return AT_RESP_PROMPT;
	}
	if (strcmp(line, "SEND OK") == 0) {
		return AT_RESP_SEND_OK;
	}
	if (strcmp(line, "SEND FAIL") == 0) {
		return AT_RESP_SEND_FAIL;
	}
	if (line[0] == '+') {
		return AT_RESP_DATA;
	}

	return AT_RESP_UNKNOWN;
}

/* Parse AT response */
int at_parse_response(const char *response, at_response_t *result) {
	if (!response || !result) {
		return -1;
	}

	memset(result, 0, sizeof(*result));
	result->type = at_get_response_type(response);

	switch (result->type) {
	case AT_RESP_OK:
	case AT_RESP_SEND_OK:
	case AT_RESP_SEND_FAIL:
	case AT_RESP_PROMPT:
		return 0;

	case AT_RESP_ERROR:
		/* Parse error code */
		if (strncmp(response, "+CME ERROR:", 11) == 0) {
			result->error_code = atoi(response + 11);
		}
		else if (strncmp(response, "+CMS ERROR:", 11) == 0) {
			result->error_code = atoi(response + 11);
		}
		else {
			result->error_code = -1;
		}
		return 0;

	case AT_RESP_DATA:
		/* Parse +CMD:data format */
		{
			const char *colon = strchr(response, ':');
			if (colon) {
				size_t cmd_len = colon - response;
				if (cmd_len >= sizeof(result->cmd)) {
					cmd_len = sizeof(result->cmd) - 1;
				}
				strncpy(result->cmd, response, cmd_len);
				result->cmd[cmd_len] = '\0';

				/* Skip spaces after colon */
				const char *data = colon + 1;
				while (*data && isspace(*data)) {
					data++;
				}
				strncpy(result->data, data, sizeof(result->data) - 1);
				result->data[sizeof(result->data) - 1] = '\0';
			}
			else {
				/* No colon - entire line is command */
				strncpy(result->cmd, response, sizeof(result->cmd) - 1);
				result->cmd[sizeof(result->cmd) - 1] = '\0';
			}
		}
		return 0;

	default:
		return -1;
	}
}

/* Check if URC (Unsolicited Result Code) */
bool at_is_urc(const char *line) {
	/* Common URC prefixes */
	static const char *urc_prefixes[] = {"+CREG:", /* Network registration */
	    "+CGREG:",                                 /* GPRS registration */
	    "+CEREG:",                                 /* EPS registration */
	    "+CSQ:",                                   /* Signal quality */
	    "+CMTI:",                                  /* New SMS notification */
	    "+CMT:",                                   /* New SMS content */
	    "+RING",                                   /* Incoming call */
	    "+CLIP:",                                  /* Caller ID */
	    "+CWJAP:",                                 /* WiFi connection status */
	    "+CWQAP:",                                 /* WiFi disconnected */
	    "+IPD,",                                   /* Received network data */
	    "RING", "NO CARRIER", "CONNECT", "CLOSED", "WIFI CONNECTED",
	    "WIFI DISCONNECT", "WIFI GOT IP", NULL};

	for (int i = 0; urc_prefixes[i] != NULL; i++) {
		if (strncmp(line, urc_prefixes[i], strlen(urc_prefixes[i])) == 0) {
			return true;
		}
	}

	return false;
}

/* Parse +IPD packet format */
int at_parse_ipd(const char *line, int *link_id, size_t *length,
    const char **data_start) {
	/* Format: +IPD,<link_id>,<length>:data or +IPD,<length>:data */
	if (strncmp(line, "+IPD,", 5) != 0) {
		return -1;
	}

	const char *p = line + 5;
	char *endptr;

	long first_num = strtol(p, &endptr, 10);

	if (*endptr == ',') {
		/* Format: +IPD,<link_id>,<length>:data */
		*link_id = (int)first_num;
		p = endptr + 1;
		*length = (size_t)strtol(p, &endptr, 10);
	}
	else if (*endptr == ':') {
		/* Format: +IPD,<length>:data */
		*link_id = 0; /* Default connection */
		*length = (size_t)first_num;
	}
	else {
		return -1;
	}

	if (*endptr != ':') {
		return -1;
	}

	*data_start = endptr + 1;

	return 0;
}

/* Extract quoted string */
int at_extract_quoted_string(const char *input, char *output, size_t out_size) {
	const char *start = strchr(input, '"');
	if (!start) {
		return -1;
	}

	start++; /* Skip opening quote */
	const char *end = strchr(start, '"');
	if (!end) {
		return -1;
	}

	size_t len = end - start;
	if (len >= out_size) {
		len = out_size - 1;
	}

	strncpy(output, start, len);
	output[len] = '\0';

	return 0;
}

/* Parse comma-separated parameters */
int at_parse_params(const char *params, char *argv[], int max_args) {
	if (!params || !argv || max_args <= 0) {
		return 0;
	}

	static char param_buffer[256];
	strncpy(param_buffer, params, sizeof(param_buffer) - 1);
	param_buffer[sizeof(param_buffer) - 1] = '\0';

	int argc = 0;
	char *p = param_buffer;
	char *start = param_buffer;
	bool in_quotes = false;

	if (*p == '\0') {
		return 0;
	}

	while (argc < max_args) {
		if (*p == '"') {
			in_quotes = !in_quotes;
			p++;
		}
		else if ((*p == ',' && !in_quotes) || *p == '\0') {
			char saved = *p;
			*p = '\0';
			argv[argc++] = start;

			if (saved == '\0') {
				break;
			}

			start = p + 1;
			p++;
		}
		else {
			p++;
		}
	}

	return argc;
}

/* Parse CME ERROR */
int at_parse_cme_error(const char *line, int *error_code) {
	if (strncmp(line, "+CME ERROR:", 11) == 0) {
		*error_code = atoi(line + 11);
		return 0;
	}
	return -1;
}

/* Parse integer value */
int at_parse_integer(const char *str, int *value) {
	if (!str || !value) {
		return -1;
	}

	char *endptr;
	long val = strtol(str, &endptr, 10);

	if (endptr == str || (*endptr != '\0' && !isspace(*endptr))) {
		return -1;
	}

	*value = (int)val;
	return 0;
}
