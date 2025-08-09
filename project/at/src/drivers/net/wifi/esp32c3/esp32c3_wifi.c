/**
 * @file
 * @brief ESP32-C3 WiFi Driver Implementation
 * 
 * @date Aug 6, 2025
 * @author Peize Li
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <at/at_client.h>
#include <mem/sysmalloc.h>
#include <net/wlan/wireless_ioctl.h>
#include <wifi/esp32c3/esp32c3_wifi.h>
#include <wifi/wifi_core.h>

#define ESP32_CONNECT_TIMEOUT_MS 20000 /* connection timeout */
#define ESP32_SCAN_TIMEOUT_MS    10000 /* scan timeout */
#define ESP32_CMD_TIMEOUT_MS     5000  /* command timeout */
#define ESP32_RESET_DELAY_MS     2000  /* reset delay */

/* Forward declarations */
static int esp32c3_hw_init(struct wifi_core *wifi);
static void esp32c3_hw_cleanup(struct wifi_core *wifi);
static int esp32c3_parse_scan_line(const char *line, struct wlan_scan_ap *ap);
static const char *esp32c3_get_scan_cmd(void);
static int esp32c3_build_connect_cmd(char *buf, size_t size, const char *ssid,
    const char *pwd);
static const char *esp32c3_get_disconnect_cmd(void);
static int esp32c3_get_ip_config(struct wifi_core *wifi, char *ip, char *mask,
    char *gw);
static bool esp32c3_parse_urc(struct wifi_core *wifi, const char *line);
static int esp32c3_get_rssi(struct wifi_core *wifi, int8_t *rssi);
static int esp32c3_parse_mode_response(const char *resp, int *mode);
int esp32c3_set_power_mode(struct wifi_core *wifi, int mode);
int esp32c3_get_power_mode(struct wifi_core *wifi, int *mode);

/* ===================================================================
 * ESP32-C3 WiFi hardware operations
 * =================================================================== */

static const struct wifi_hw_ops esp32c3_ops = {
	.hw_init = esp32c3_hw_init,
	.hw_cleanup = esp32c3_hw_cleanup,
	.parse_scan_line = esp32c3_parse_scan_line,
	.get_scan_cmd = esp32c3_get_scan_cmd,
	.build_connect_cmd = esp32c3_build_connect_cmd,
	.get_disconnect_cmd = esp32c3_get_disconnect_cmd,
	.get_ip_config = esp32c3_get_ip_config,
	.parse_urc = esp32c3_parse_urc,
	.get_rssi = esp32c3_get_rssi,
	.parse_mode_response = esp32c3_parse_mode_response,
	.set_power_mode = esp32c3_set_power_mode,
	.get_power_mode = esp32c3_get_power_mode,
	.timeouts = {
		.scan_ms = ESP32_SCAN_TIMEOUT_MS,
		.connect_ms = ESP32_CONNECT_TIMEOUT_MS,
		.cmd_ms = ESP32_CMD_TIMEOUT_MS
	},
	
	.urc_patterns = {
		.connected = "WIFI CONNECTED",
		.got_ip = "WIFI GOT IP", 
		.disconnected = "WIFI DISCONNECT"
	},

	.basic_cmds = {
		.scan = "AT+CWLAP",
		.disconnect = "AT+CWQAP"
	},

	.mode_cmds = {
		.get_mode = "AT+CWMODE?",
		.set_mode_fmt = "AT+CWMODE=%d"
	}
};

/* Helper functions */
static int esp32_error_to_errno(int esp_error) {
	switch (esp_error) {
	case 1: /* timeout */
		return -ETIMEDOUT;
	case 2: /* wrong password */
		return -EACCES;
	case 3: /* AP not found */
		return -ENOENT;
	case 4: /* connection failed */
		return -ECONNREFUSED;
	default:
		return -EIO;
	}
}

/* Hardware initialization and cleanup */
static int esp32c3_hw_init(struct wifi_core *wifi) {
	struct esp32c3_wifi *esp32 = (struct esp32c3_wifi *)wifi->hw_priv;
	char resp[256];
	int ret;

	printf("esp32c3: initializing WiFi module\n");

	/* Test AT communication */
	printf("esp32c3_hw_init: sending AT command\n");
	ret = wifi_core_send_cmd(wifi, "AT", resp, sizeof(resp), 1000);
	if (ret != 0) {
		printf("esp32c3_hw_init: AT failed with ret=%d\n", ret);
		return ret;
	}
	printf("esp32c3_hw_init: AT response: %s\n", resp);

	/* Reset module */
	wifi_core_send_cmd(wifi, "AT+RST", resp, sizeof(resp), 1000);
	usleep(ESP32_RESET_DELAY_MS * 500);

	/* Clear any boot messages */
	at_client_process_rx(wifi->at_client);

	/* Disable echo */
	ret = wifi_core_send_cmd(wifi, "ATE0", resp, sizeof(resp), 1000);
	if (ret != 0) {
		printf("esp32c3: failed to disable echo\n");
		return ret;
	}

	/* Query version */
	ret = wifi_core_send_cmd(wifi, "AT+GMR", resp, sizeof(resp), 2000);
	if (ret == 0) {
		char *ver = strstr(resp, "AT version:");
		if (ver) {
			sscanf(ver, "AT version:%63s", esp32->fw_version);
			printf("esp32c3: firmware %s\n", esp32->fw_version);
		}
	}

	esp32->power_mode = WLAN_PWR_ACTIVE;
	esp32->initialized = true;
	printf("esp32c3: initialization complete\n");

	return 0;
}

static void esp32c3_hw_cleanup(struct wifi_core *wifi) {
	struct esp32c3_wifi *esp32 = (struct esp32c3_wifi *)wifi->hw_priv;

	if (esp32->initialized) {
		wifi_core_send_cmd(wifi, "AT+CWQAP", NULL, 0, 1000);
		esp32->initialized = false;
	}
}

/* Command builders and getters */
static const char *esp32c3_get_scan_cmd(void) {
	return "AT+CWLAP";
}

static int esp32c3_build_connect_cmd(char *buf, size_t size, const char *ssid,
    const char *pwd) {
	if (!ssid || !buf) {
		return -EINVAL;
	}

	if (pwd && pwd[0]) {
		snprintf(buf, size, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
	}
	else {
		snprintf(buf, size, "AT+CWJAP=\"%s\",\"\"", ssid);
	}

	return 0;
}

static const char *esp32c3_get_disconnect_cmd(void) {
	return "AT+CWQAP";
}

/* Response parsers */
static int esp32c3_parse_scan_line(const char *line, struct wlan_scan_ap *ap) {
	char work_buf[256];
	char *ptr, *start;
	int field = 0;

	memset(ap, 0, sizeof(*ap));

	if (strncmp(line, "+CWLAP:", 7) != 0) {
		return -1;
	}

	/* Copy to work buffer, remove prefix */
	strncpy(work_buf, line + 7, sizeof(work_buf) - 1);
	work_buf[sizeof(work_buf) - 1] = '\0';

	/* Remove leading '(' and trailing ')' */
	ptr = work_buf;
	while (*ptr == ' ' || *ptr == '(')
		ptr++;
	start = ptr;

	ptr = strrchr(start, ')');
	if (ptr)
		*ptr = '\0';

	/* Manual parsing to avoid strtok state issues */
	ptr = start;

	while (*ptr && field < 5) {
		while (*ptr == ' ')
			ptr++;

		switch (field) {
		case 0: /* Encryption type */
		{
			char *endptr;
			long val = strtol(ptr, &endptr, 10);
			ap->security = (val == 0) ? 0 : 3;
			ptr = endptr;
			if (*ptr == ',')
				ptr++;
		} break;

		case 1: /* SSID */
		{
			if (*ptr == '"')
				ptr++;
			char *ssid_start = ptr;
			char *ssid_end = strchr(ptr, '"');
			if (!ssid_end) {
				printf("esp32c3: SSID parse error\n");
				return -1;
			}
			size_t len = ssid_end - ssid_start;
			if (len > 32)
				len = 32;
			memcpy(ap->ssid, ssid_start, len);
			ap->ssid[len] = '\0';
			ptr = ssid_end + 1;
			if (*ptr == ',')
				ptr++;
		} break;

		case 2: /* RSSI */
		{
			char *endptr;
			long val = strtol(ptr, &endptr, 10);
			ap->rssi = (int8_t)val;
			if (ap->rssi > 0)
				ap->rssi = -ap->rssi;
			if (ap->rssi < -100)
				ap->rssi = -100;
			ptr = endptr;
			if (*ptr == ',')
				ptr++;
		} break;

		case 3: /* MAC address */
		{
			if (*ptr == '"')
				ptr++;
			for (int i = 0; i < 6; i++) {
				char *endptr;
				unsigned long byte = strtoul(ptr, &endptr, 16);
				if (byte > 255) {
					printf("esp32c3: Invalid MAC byte\n");
					return -1;
				}
				ap->bssid[i] = (uint8_t)byte;
				ptr = endptr;
				if (i < 5) {
					if (*ptr == ':') {
						ptr++;
					}
					else {
						printf("esp32c3: MAC format error at byte %d\n", i);
						return -1;
					}
				}
			}
			if (*ptr == '"')
				ptr++;
			if (*ptr == ',')
				ptr++;
		} break;

		case 4: /* Channel */
		{
			char *endptr;
			long val = strtol(ptr, &endptr, 10);
			ap->channel = (uint8_t)val;
			if (ap->channel < 1)
				ap->channel = 1;
			if (ap->channel > 14)
				ap->channel = 14;
			ptr = endptr;
		} break;
		}

		field++;
	}

	if (field < 5) {
		printf("esp32c3: incomplete parse, only %d fields\n", field);
		return -1;
	}

	printf("esp32c3: parsed: ssid='%s', rssi=%d, ch=%d, sec=%d, "
	       "mac=%02x:%02x:%02x:%02x:%02x:%02x\n",
	    ap->ssid, ap->rssi, ap->channel, ap->security, ap->bssid[0],
	    ap->bssid[1], ap->bssid[2], ap->bssid[3], ap->bssid[4], ap->bssid[5]);

	return 0;
}

static int esp32c3_parse_mode_response(const char *resp, int *mode) {
	char *p = strstr(resp, "+CWMODE:");
	if (p) {
		*mode = atoi(p + 8);
		return 0;
	}
	return -EINVAL;
}

static bool esp32c3_parse_urc(struct wifi_core *wifi, const char *line) {
	struct esp32c3_wifi *esp32 = (struct esp32c3_wifi *)wifi->hw_priv;

	/* Connection error code */
	if (strstr(line, "+CWJAP:")) {
		int error_code;
		if (sscanf(line, "+CWJAP:%d", &error_code) == 1) {
			esp32->last_error_code = error_code;

			int errno_val = esp32_error_to_errno(error_code);

			wifi->connect_result = errno_val;
			wifi->connect_done = true;

			const char *error_msg = "";
			switch (error_code) {
			case 1:
				error_msg = "connection timeout";
				break;
			case 2:
				error_msg = "wrong password";
				break;
			case 3:
				error_msg = "cannot find AP";
				break;
			case 4:
				error_msg = "connection failed";
				break;
			default:
				error_msg = "unknown error";
				break;
			}
			printf("esp32c3: connection failed - %s (code=%d)\n", error_msg,
			    error_code);
		}
		return true;
	}

	/* WiFi disconnect reason */
	if (strstr(line, "WIFI DISCONNECT")) {
		int reason = 0;
		if (sscanf(line, "WIFI DISCONNECT,reason:%d", &reason) == 1) {
			printf("esp32c3: disconnected with reason %d\n", reason);
		}
		return true;
	}

	/* Other ESP32-specific messages */
	if (strstr(line, "+CWSTATE:") || strstr(line, "+DIST_STA_IP:")) {
		return true;
	}

	return false;
}

/* Network configuration and status */
static int esp32c3_get_ip_config(struct wifi_core *wifi, char *ip, char *mask,
    char *gw) {
	char resp[512];
	char *line;
	int ret;

	ret = wifi_core_send_cmd(wifi, "AT+CIPSTA?", resp, sizeof(resp), 2000);
	if (ret != 0) {
		return ret;
	}

	line = strtok(resp, "\r\n");
	while (line) {
		char *colon = strstr(line, ":ip:");
		if (colon) {
			char *start = strchr(colon, '"');
			if (start) {
				start++;
				char *end = strchr(start, '"');
				if (end) {
					size_t len = end - start;
					if (len < 16) {
						memcpy(ip, start, len);
						ip[len] = '\0';
					}
				}
			}
		}

		colon = strstr(line, ":gateway:");
		if (colon) {
			char *start = strchr(colon, '"');
			if (start) {
				start++;
				char *end = strchr(start, '"');
				if (end) {
					size_t len = end - start;
					if (len < 16) {
						memcpy(gw, start, len);
						gw[len] = '\0';
					}
				}
			}
		}

		colon = strstr(line, ":netmask:");
		if (colon) {
			char *start = strchr(colon, '"');
			if (start) {
				start++;
				char *end = strchr(start, '"');
				if (end) {
					size_t len = end - start;
					if (len < 16) {
						memcpy(mask, start, len);
						mask[len] = '\0';
					}
				}
			}
		}

		line = strtok(NULL, "\r\n");
	}

	return 0;
}

static int esp32c3_get_rssi(struct wifi_core *wifi, int8_t *rssi) {
	char resp[256];
	char *line;
	int ret;

	ret = wifi_core_send_cmd(wifi, "AT+CWJAP?", resp, sizeof(resp), 2000);
	if (ret != 0) {
		return ret;
	}

	/* Parse format: +CWJAP:"ssid","bssid",channel,rssi */
	line = strstr(resp, "+CWJAP:");
	if (line) {
		char *ptr = line;
		int comma_count = 0;

		/* Find the third comma */
		while (*ptr != '\0' && comma_count < 3) {
			if (*ptr == ',') {
				comma_count++;
			}
			ptr++;
		}

		if (comma_count == 3) {
			long val = strtol(ptr, NULL, 10);
			*rssi = (int8_t)val;
			return 0;
		}
	}

	return -ENODATA;
}

/* Power management */
int esp32c3_set_power_mode(struct wifi_core *wifi, int mode) {
	char cmd[32];
	char resp[256];
	int sleep_mode;

	/* Map to ESP32 sleep mode */
	switch (mode) {
	case WLAN_PWR_ACTIVE:
		sleep_mode = 0; /* Disable sleep */
		break;
	case WLAN_PWR_SAVE:
		sleep_mode = 1; /* Modem-sleep */
		break;
	case WLAN_PWR_DEEP_SAVE:
		sleep_mode = 2; /* Light-sleep */
		break;
	default:
		return -EINVAL;
	}

	snprintf(cmd, sizeof(cmd), "AT+SLEEP=%d", sleep_mode);
	int ret = wifi_core_send_cmd(wifi, cmd, resp, sizeof(resp), 2000);

	if (ret == 0) {
		struct esp32c3_wifi *esp32 = (struct esp32c3_wifi *)wifi->hw_priv;
		esp32->power_mode = mode;

		printf("esp32c3: power mode set to %s\n",
		    mode == WLAN_PWR_ACTIVE ? "active"
		    : mode == WLAN_PWR_SAVE ? "power save"
		                            : "deep save");
	}

	return ret;
}

int esp32c3_get_power_mode(struct wifi_core *wifi, int *mode) {
	struct esp32c3_wifi *esp32 = (struct esp32c3_wifi *)wifi->hw_priv;
	char resp[256];
	int ret;

	ret = wifi_core_send_cmd(wifi, "AT+SLEEP?", resp, sizeof(resp), 1000);
	if (ret == 0) {
		char *p = strstr(resp, "+SLEEP:");
		if (p) {
			int sleep_mode = atoi(p + 7);
			switch (sleep_mode) {
			case 0:
				*mode = WLAN_PWR_ACTIVE;
				break;
			case 1:
				*mode = WLAN_PWR_SAVE;
				break;
			case 2:
				*mode = WLAN_PWR_DEEP_SAVE;
				break;
			default:
				return -EINVAL;
			}
			esp32->power_mode = *mode;
			return 0;
		}
	}

	return -EIO;
}

int esp32c3_wifi_init_fd(struct net_device *netdev, int fd) {
	struct esp32c3_wifi *esp32;
	struct wlan_device *wdev;
	int ret;

	printf("esp32c3_wifi_init_fd: entering with fd=%d\n", fd);

	if (!netdev || fd < 0) {
		printf("esp32c3_wifi_init_fd: invalid params\n");
		return -EINVAL;
	}

	/* Set wireless_ops before wlan_device_init */
	netdev->wireless_ops = &wifi_core_ops;

	/* Initialize WLAN device if needed */
	if (!(netdev->flags & IFF_WIRELESS)) {
		ret = wlan_device_init(netdev);
		if (ret != 0) {
			printf("esp32c3_wifi_init_fd: wlan_device_init failed with %d\n", ret);
			netdev->wireless_ops = NULL;
			return ret;
		}
		printf("esp32c3_wifi_init_fd: wlan_device initialized\n");
	}

	wdev = netdev->wireless_priv;
	if (!wdev) {
		return -EINVAL;
	}

	/* Allocate ESP32 driver structure */
	esp32 = sysmalloc(sizeof(*esp32));
	if (!esp32) {
		return -ENOMEM;
	}
	memset(esp32, 0, sizeof(*esp32));

	/* Create AT client */
	esp32->at_client = sysmalloc(sizeof(at_client_t));
	if (!esp32->at_client) {
		sysfree(esp32);
		return -ENOMEM;
	}

	/* Initialize AT client with FD */
	ret = at_client_init_fd(esp32->at_client, fd);
	if (ret != 0) {
		printf("esp32c3: failed to init AT client with fd %d\n", fd);
		sysfree(esp32->at_client);
		sysfree(esp32);
		return ret;
	}

	/* Mark that we don't own this fd */
	esp32->owns_at_client_fd = false;

	/* Configure AT client */
	at_client_set_mode(esp32->at_client, AT_CLIENT_MODE_NORMAL);

#ifdef CONFIG_ESP32C3_AT_DEBUG
	at_client_enable_debug(esp32->at_client, true);
	printf("esp32c3: AT debug enabled\n");
#endif

	/* Initialize WiFi Core */
	ret = wifi_core_setup(&esp32->core, netdev, esp32->at_client, &esp32c3_ops,
	    esp32);
	if (ret != 0) {
		printf("esp32c3: failed to init wifi core\n");
		at_client_close(esp32->at_client);
		sysfree(esp32->at_client);
		sysfree(esp32);
		return ret;
	}

	/* Register driver type */
	ret = wlan_device_register_driver(netdev, WLAN_DRV_WIFI_CORE, &esp32->core);
	if (ret != 0) {
		printf("esp32c3: failed to register driver type\n");
		wifi_core_cleanup(&esp32->core);
		at_client_close(esp32->at_client);
		sysfree(esp32->at_client);
		sysfree(esp32);
		return ret;
	}

	printf("esp32c3: driver initialized on fd %d\n", fd);
	return 0;
}

int esp32c3_wifi_init(struct net_device *netdev, const char *uart_path) {
	int fd;
	int ret;

	if (!netdev || !uart_path) {
		return -EINVAL;
	}

	fd = open(uart_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0) {
		printf("esp32c3: failed to open %s: %s\n", uart_path, strerror(errno));
		return -EIO;
	}

	ret = esp32c3_wifi_init_fd(netdev, fd);
	if (ret != 0) {
		close(fd);
		return ret;
	}

	/* Mark that we own this fd */
	struct wlan_device *wdev = netdev->wireless_priv;
	struct wifi_core *wifi = wdev->priv;
	struct esp32c3_wifi *esp32 = wifi->hw_priv;
	esp32->owns_at_client_fd = true;

	printf("esp32c3: driver initialized on %s\n", uart_path);
	return 0;
}

void esp32c3_wifi_cleanup(struct net_device *netdev) {
	struct wlan_device *wdev;
	struct wifi_core *wifi;
	struct esp32c3_wifi *esp32;

	if (!netdev || !(netdev->flags & IFF_WIRELESS)) {
		return;
	}

	wdev = netdev->wireless_priv;
	if (!wdev || !wdev->priv) {
		return;
	}

	wifi = (struct wifi_core *)wdev->priv;
	esp32 = (struct esp32c3_wifi *)wifi->hw_priv;

	wifi_core_cleanup(wifi);

	if (esp32->at_client) {
		at_client_close(esp32->at_client);
		sysfree(esp32->at_client);
		esp32->at_client = NULL;
	}

	sysfree(esp32);

	wlan_device_cleanup(netdev);

	printf("esp32c3: driver cleaned up\n");
}
