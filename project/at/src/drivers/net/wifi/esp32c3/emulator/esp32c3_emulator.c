/**
 * @file
 * @brief ESP32-C3 AT Emulator Implementation 
 * 
 * @date July 31, 2025
 * @author Peize Li
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/thread/sync/mutex.h>
#include <kernel/time/timer.h>
#include <wifi/esp32c3/emulator/esp32c3_emulator.h>

/* WiFi connection simulation delay (ms) */
#define WIFI_CONNECT_DELAY_MS 200
#define WIFI_IP_DELAY_MS      50

static esp32c3_emu_data_t *get_esp32_data(at_emulator_t *emu) {
	return (esp32c3_emu_data_t *)at_emulator_get_user_data(emu);
}

/* Device-specific poll callback for ESP32-C3 */
static const char *esp32c3_device_poll(at_emulator_t *emu) {
	esp32c3_emu_data_t *data = get_esp32_data(emu);
	static char buffer[256];

	if (!data) {
		return NULL;
	}

	/* Handle post-reset "READY" message */
	mutex_lock(&data->state_mutex);
	if (data->rst_pending) {
		data->rst_pending = false;
		strcpy(buffer, "\r\nREADY\r\n");
		mutex_unlock(&data->state_mutex);
		return buffer;
	}
	mutex_unlock(&data->state_mutex);

	return NULL;
}

/* Safe timer stop with mutex protection */
static void emu_timer_stop(esp32c3_emu_data_t *data) {
	mutex_lock(&data->timer_mutex);

	if (data->timer_active) {
		timer_stop(&data->conn_timer);
		data->timer_active = false;
	}
	if (data->ip_timer_active) {
		timer_stop(&data->ip_timer);
		data->ip_timer_active = false;
	}

	mutex_unlock(&data->timer_mutex);
}

/* Timer callback for WiFi connection stages */
static void wifi_connect_timer_handler(struct sys_timer *timer, void *param) {
	esp32c3_emu_data_t *data = (esp32c3_emu_data_t *)param;

	(void)timer;

	mutex_lock(&data->timer_mutex);
	mutex_lock(&data->state_mutex);

	switch (data->wifi_state) {
	case ESP32_WIFI_CONNECTING:
		data->wifi_state = ESP32_WIFI_CONNECTED;
		at_emulator_push_urc(data->base_emu, "\r\nWIFI CONNECTED\r\n");

		/* Schedule IP acquisition */
		if (!data->ip_timer_active) {
			timer_init_start_msec(&data->ip_timer, TIMER_ONESHOT,
			    WIFI_IP_DELAY_MS, wifi_connect_timer_handler, data);
			data->ip_timer_active = true;
		}
		data->timer_active = false;
		break;

	case ESP32_WIFI_CONNECTED:
		data->wifi_state = ESP32_WIFI_GOT_IP;
		strcpy(data->ip_addr, "192.168.1.100");
		strcpy(data->gateway, "192.168.1.1");
		strcpy(data->netmask, "255.255.255.0");

		at_emulator_push_urc(data->base_emu, "\r\nWIFI GOT IP\r\n");
		data->ip_timer_active = false;
		break;

	default:
		data->timer_active = false;
		data->ip_timer_active = false;
		break;
	}

	mutex_unlock(&data->state_mutex);
	mutex_unlock(&data->timer_mutex);
}

/* AT+RST - Reset module */
static at_result_t cmd_rst(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	esp32c3_emu_data_t *data = get_esp32_data(emu);

	(void)type;
	(void)params;
	(void)response;
	(void)resp_size;

	emu_timer_stop(data);

	mutex_lock(&data->state_mutex);
	data->wifi_state = ESP32_WIFI_IDLE;
	data->wifi_mode = 1; /* Default STA mode */
	memset(data->current_ssid, 0, sizeof(data->current_ssid));
	memset(data->current_password, 0, sizeof(data->current_password));
	memset(data->ip_addr, 0, sizeof(data->ip_addr));
	memset(data->gateway, 0, sizeof(data->gateway));
	memset(data->netmask, 0, sizeof(data->netmask));
	data->tcp_state.connected = false;
	mutex_unlock(&data->state_mutex);

	data->rst_pending = true;

	return AT_OK;
}

/* AT+CWMODE - Set/Query WiFi mode */
static at_result_t cmd_cwmode(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	esp32c3_emu_data_t *data = get_esp32_data(emu);

	switch (type) {
	case AT_CMD_SET:
		if (params && *params) {
			int mode = atoi(params);
			if (mode >= 1 && mode <= 3) {
				mutex_lock(&data->state_mutex);
				data->wifi_mode = mode;
				mutex_unlock(&data->state_mutex);
				return AT_OK;
			}
		}
		return AT_ERROR;

	case AT_CMD_READ:
		mutex_lock(&data->state_mutex);
		snprintf(response, resp_size, "+CWMODE:%d", data->wifi_mode);
		mutex_unlock(&data->state_mutex);
		return AT_OK;

	case AT_CMD_TEST:
		snprintf(response, resp_size, "+CWMODE:(1-3)");
		return AT_OK;

	case AT_CMD_EXEC:
		return AT_ERROR;

	default:
		return AT_ERROR;
	}
}

/* AT+CWJAP - Connect to AP */
static at_result_t cmd_cwjap(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	esp32c3_emu_data_t *data = get_esp32_data(emu);

	switch (type) {
	case AT_CMD_SET:
		if (params) {
			char ssid[33] = {0};
			char pwd[65] = {0};
			char temp[128];

			strncpy(temp, params, sizeof(temp) - 1);
			temp[sizeof(temp) - 1] = '\0';

			/* Parse SSID and password from quoted parameters */
			char *p1 = strtok(temp, "\"");
			if (p1) {
				strncpy(ssid, p1, sizeof(ssid) - 1);

				strtok(NULL, "\"");
				char *p2 = strtok(NULL, "\"");
				if (p2) {
					strncpy(pwd, p2, sizeof(pwd) - 1);
				}
			}

			if (strlen(ssid) > 0) {
				/* Check if SSID exists in scan results */
				bool ssid_found = false;
				mutex_lock(&data->state_mutex);
				for (int i = 0; i < data->scan_count; i++) {
					if (strcmp(data->scan_results[i].ssid, ssid) == 0) {
						ssid_found = true;
						break;
					}
				}
				mutex_unlock(&data->state_mutex);

				/* Accept any SSID starting with "Test" for simulation */
				if (!ssid_found && strncmp(ssid, "Test", 4) == 0) {
					ssid_found = true;
				}

				if (!ssid_found) {
					return AT_ERROR;
				}

				/* Disconnect if already connected */
				mutex_lock(&data->state_mutex);
				if (data->wifi_state >= ESP32_WIFI_CONNECTED) {
					data->wifi_state = ESP32_WIFI_DISCONNECTED;
				}
				mutex_unlock(&data->state_mutex);

				emu_timer_stop(data);

				/* Start new connection */
				mutex_lock(&data->state_mutex);
				strncpy(data->current_ssid, ssid, sizeof(data->current_ssid) - 1);
				strncpy(data->current_password, pwd,
				    sizeof(data->current_password) - 1);
				data->wifi_state = ESP32_WIFI_CONNECTING;
				mutex_unlock(&data->state_mutex);

				mutex_lock(&data->timer_mutex);
				timer_init_start_msec(&data->conn_timer, TIMER_ONESHOT,
				    WIFI_CONNECT_DELAY_MS, wifi_connect_timer_handler, data);
				data->timer_active = true;
				mutex_unlock(&data->timer_mutex);

				return AT_OK;
			}
		}
		return AT_ERROR;

	case AT_CMD_READ:
		mutex_lock(&data->state_mutex);
		if (data->wifi_state >= ESP32_WIFI_CONNECTED
		    && strlen(data->current_ssid) > 0) {
			snprintf(response, resp_size,
			    "+CWJAP:\"%s\",\"%02x:%02x:%02x:%02x:%02x:%02x\",%d,%d",
			    data->current_ssid, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, /* Fake BSSID */
			    6, -45); /* channel, rssi */
		}
		else {
			response[0] = '\0';
		}
		mutex_unlock(&data->state_mutex);
		return AT_OK;

	case AT_CMD_TEST:
		snprintf(response, resp_size, "+CWJAP:<ssid>,<pwd>[,<bssid>][,<pci_en>]");
		return AT_OK;

	default:
		return AT_ERROR;
	}
}

/* AT+CWQAP - Disconnect from AP */
static at_result_t cmd_cwqap(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	esp32c3_emu_data_t *data = get_esp32_data(emu);

	(void)type;
	(void)params;
	(void)response;
	(void)resp_size;

	if (type != AT_CMD_EXEC) {
		return AT_ERROR;
	}

	emu_timer_stop(data);

	mutex_lock(&data->state_mutex);
	if (data->wifi_state >= ESP32_WIFI_CONNECTED) {
		data->wifi_state = ESP32_WIFI_DISCONNECTED;
		memset(data->current_ssid, 0, sizeof(data->current_ssid));
		memset(data->current_password, 0, sizeof(data->current_password));
		memset(data->ip_addr, 0, sizeof(data->ip_addr));
		memset(data->gateway, 0, sizeof(data->gateway));
		memset(data->netmask, 0, sizeof(data->netmask));

		at_emulator_push_urc(data->base_emu, "\r\nWIFI DISCONNECT\r\n");
	}
	mutex_unlock(&data->state_mutex);

	return AT_OK;
}

/* AT+CWLAP - List available APs */
static at_result_t cmd_cwlap(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	esp32c3_emu_data_t *data = get_esp32_data(emu);

	(void)params;

	if (type != AT_CMD_EXEC && type != AT_CMD_SET) {
		return AT_ERROR;
	}

	int len = 0;
	mutex_lock(&data->state_mutex);
	for (int i = 0; i < data->scan_count && len < (int)resp_size - 100; i++) {
		len += snprintf(response + len, resp_size - len,
		    "+CWLAP:(%d,\"%s\",%d,\"%02x:%02x:%02x:%02x:%02x:%02x\",%d)\r\n",
		    data->scan_results[i].enc, data->scan_results[i].ssid,
		    data->scan_results[i].rssi, 0x00, 0x11, 0x22, 0x33, 0x44,
		    i, /* Fake BSSID */
		    data->scan_results[i].channel);
	}
	mutex_unlock(&data->state_mutex);

	/* Remove last \r\n */
	if (len >= 2) {
		response[len - 2] = '\0';
	}

	return AT_OK;
}

/* AT+CIPSTA? - Query station IP */
static at_result_t cmd_cipsta(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	esp32c3_emu_data_t *data = get_esp32_data(emu);

	(void)params;

	if (type == AT_CMD_READ) {
		mutex_lock(&data->state_mutex);
		if (data->wifi_state == ESP32_WIFI_GOT_IP) {
			snprintf(response, resp_size,
			    "+CIPSTA:ip:\"%s\"\r\n"
			    "+CIPSTA:gateway:\"%s\"\r\n"
			    "+CIPSTA:netmask:\"%s\"",
			    data->ip_addr, data->gateway, data->netmask);
		}
		else {
			snprintf(response, resp_size,
			    "+CIPSTA:ip:\"0.0.0.0\"\r\n"
			    "+CIPSTA:gateway:\"0.0.0.0\"\r\n"
			    "+CIPSTA:netmask:\"0.0.0.0\"");
		}
		mutex_unlock(&data->state_mutex);
		return AT_OK;
	}

	return AT_ERROR;
}

/* AT+CIPSTART - Establish TCP/UDP connection */
static at_result_t cmd_cipstart(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	esp32c3_emu_data_t *data = get_esp32_data(emu);

	(void)response;
	(void)resp_size;

	if (type == AT_CMD_SET && params) {
		mutex_lock(&data->state_mutex);
		if (data->wifi_state != ESP32_WIFI_GOT_IP) {
			mutex_unlock(&data->state_mutex);
			return AT_ERROR;
		}

		/* Simplified parsing - simulate success */
		data->tcp_state.connected = true;
		strcpy(data->tcp_state.remote_ip, "192.168.1.10");
		data->tcp_state.remote_port = 8080;

		at_emulator_push_urc(data->base_emu, "\r\nCONNECT\r\n");
		mutex_unlock(&data->state_mutex);

		return AT_OK;
	}

	return AT_ERROR;
}

/* AT+CIPSEND - Send data */
static at_result_t cmd_cipsend(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	esp32c3_emu_data_t *data = get_esp32_data(emu);

	(void)type;
	(void)params;

	mutex_lock(&data->state_mutex);
	if (data->tcp_state.connected) {
		strcpy(response, "> ");
		mutex_unlock(&data->state_mutex);
		return AT_OK;
	}
	mutex_unlock(&data->state_mutex);

	(void)resp_size;
	return AT_ERROR;
}

/* AT+SLEEP - Set sleep mode */
static at_result_t cmd_sleep(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	esp32c3_emu_data_t *data = get_esp32_data(emu);

	switch (type) {
	case AT_CMD_SET:
		if (params && *params) {
			int mode = atoi(params);
			if (mode >= 0 && mode <= 2) {
				mutex_lock(&data->state_mutex);
				data->sleep_mode = mode;
				mutex_unlock(&data->state_mutex);
				return AT_OK;
			}
		}
		return AT_ERROR;

	case AT_CMD_READ:
		mutex_lock(&data->state_mutex);
		snprintf(response, resp_size, "+SLEEP:%d", data->sleep_mode);
		mutex_unlock(&data->state_mutex);
		return AT_OK;

	default:
		return AT_ERROR;
	}
}

/* AT+CIPSTAMAC - Get station MAC address */
static at_result_t cmd_cipstamac(at_emulator_t *emu, at_cmd_type_t type,
	const char *params, char *response, size_t resp_size) {
	if (type == AT_CMD_READ) {
		snprintf(response, resp_size, 
							"+CIPSTAMAC:\"AA:BB:CC:DD:EE:FF\"");
		return AT_OK;
}
	return AT_ERROR;
}


/* Initialize ESP32-C3 emulator */
at_emulator_t *esp32c3_emulator_create(void) {
	at_emulator_t *emu = at_emulator_create();
	if (!emu) {
		return NULL;
	}

	esp32c3_emu_data_t *data = calloc(1, sizeof(esp32c3_emu_data_t));
	if (!data) {
		at_emulator_destroy(emu);
		return NULL;
	}

	mutex_init(&data->timer_mutex);
	mutex_init(&data->state_mutex);

	data->wifi_mode = 1; /* STA mode */
	data->wifi_state = ESP32_WIFI_IDLE;
	data->base_emu = emu;
	data->timer_active = false;
	data->ip_timer_active = false;
	data->rst_pending = false;

	/* Initialize fake scan results */
	data->scan_count = 3;
	strcpy(data->scan_results[0].ssid, "TestAP_1");
	data->scan_results[0].rssi = -45;
	data->scan_results[0].channel = 1;
	data->scan_results[0].enc = 3; /* WPA2 */

	strcpy(data->scan_results[1].ssid, "TestAP_2");
	data->scan_results[1].rssi = -60;
	data->scan_results[1].channel = 6;
	data->scan_results[1].enc = 0; /* Open */

	strcpy(data->scan_results[2].ssid, "TestAP_3");
	data->scan_results[2].rssi = -75;
	data->scan_results[2].channel = 11;
	data->scan_results[2].enc = 3; /* WPA2 */

	at_emulator_set_device_info(emu, "ESP32-C3_Emulator", "Embox", "ESP32-C3",
	    "AT 1.0");
	at_emulator_set_user_data(emu, data);
	emu->device_poll = esp32c3_device_poll;

	/* Register ESP32-C3 specific commands */
	at_emulator_register_command(emu, "AT+RST", cmd_rst);
	at_emulator_register_command(emu, "AT+CWMODE", cmd_cwmode);
	at_emulator_register_command(emu, "AT+CWJAP", cmd_cwjap);
	at_emulator_register_command(emu, "AT+CWQAP", cmd_cwqap);
	at_emulator_register_command(emu, "AT+CWLAP", cmd_cwlap);
	at_emulator_register_command(emu, "AT+CIPSTA", cmd_cipsta);
	at_emulator_register_command(emu, "AT+CIPSTART", cmd_cipstart);
	at_emulator_register_command(emu, "AT+CIPSEND", cmd_cipsend);
	at_emulator_register_command(emu, "AT+SLEEP", cmd_sleep);
	at_emulator_register_command(emu, "AT+CIPSTAMAC", cmd_cipstamac);

	return emu;
}

/* Destroy ESP32-C3 emulator */
void esp32c3_emulator_destroy(at_emulator_t *emu) {
	if (emu) {
		esp32c3_emu_data_t *data = get_esp32_data(emu);
		if (data) {
			emu_timer_stop(data);
			free(data);
		}
		at_emulator_destroy(emu);
	}
}
