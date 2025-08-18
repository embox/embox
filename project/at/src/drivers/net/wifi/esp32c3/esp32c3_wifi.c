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
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <mem/sysmalloc.h>
#include <wifi/esp32c3/esp32c3_wifi.h>
#include <wifi/wifi_core.h>

/* Default timeouts */
#define ESP32_DEFAULT_CONNECT_TIMEOUT_MS 20000
#define ESP32_DEFAULT_SCAN_TIMEOUT_MS    10000
#define ESP32_DEFAULT_CMD_TIMEOUT_MS     5000
#define ESP32_RESET_DELAY_MS             2000
#define ESP32_INIT_DELAY_MS              1000

/* ESP32-C3 private data structure */
struct esp32c3_private {
	/* Configuration */
	struct esp32c3_config config;

	/* Status information */
	struct esp32c3_status status;

	/* AT client management */
	at_client_t *at_client;
	bool owns_at_fd;

	/* Internal state */
	bool initialized;
	int wifi_mode;
	int power_mode;
	bool auto_connect;
	bool dhcp_enabled;

	/* WiFi core reference */
	struct wifi_core *wifi_core;
};

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
static int esp32c3_hw_set_power_mode(struct wifi_core *wifi, int mode);
static int esp32c3_hw_get_power_mode(struct wifi_core *wifi, int *mode);

/* WiFi hardware operations table */
static const struct wifi_hw_ops esp32c3_ops = {.hw_init = esp32c3_hw_init,
    .hw_cleanup = esp32c3_hw_cleanup,
    .parse_scan_line = esp32c3_parse_scan_line,
    .get_scan_cmd = esp32c3_get_scan_cmd,
    .build_connect_cmd = esp32c3_build_connect_cmd,
    .get_disconnect_cmd = esp32c3_get_disconnect_cmd,
    .get_ip_config = esp32c3_get_ip_config,
    .parse_urc = esp32c3_parse_urc,
    .get_rssi = esp32c3_get_rssi,
    .parse_mode_response = esp32c3_parse_mode_response,
    .set_power_mode = esp32c3_hw_set_power_mode,
    .get_power_mode = esp32c3_hw_get_power_mode,
    .timeouts = {.scan_ms = ESP32_DEFAULT_SCAN_TIMEOUT_MS,
        .connect_ms = ESP32_DEFAULT_CONNECT_TIMEOUT_MS,
        .cmd_ms = ESP32_DEFAULT_CMD_TIMEOUT_MS},
    .urc_patterns = {.connected = "WIFI CONNECTED",
        .got_ip = "WIFI GOT IP",
        .disconnected = "WIFI DISCONNECT"},
    .basic_cmds = {.scan = "AT+CWLAP", .disconnect = "AT+CWQAP"},
    .mode_cmds = {.get_mode = "AT+CWMODE?", .set_mode_fmt = "AT+CWMODE=%d"}};

/* ===================================================================
 * Helper Functions
 * =================================================================== */

static int esp32_error_to_errno(int esp_error) {
	switch (esp_error) {
	case ESP32_ERR_TIMEOUT:
		return -ETIMEDOUT;
	case ESP32_ERR_WRONG_PASS:
		return -EACCES;
	case ESP32_ERR_NO_AP:
		return -ENOENT;
	case ESP32_ERR_CONN_FAIL:
		return -ECONNREFUSED;
	case ESP32_ERR_DHCP_FAIL:
		return -EHOSTUNREACH;
	default:
		return -EIO;
	}
}

static struct esp32c3_private *wiphy_to_esp32(struct wiphy *wiphy) {
	struct wifi_core *wifi = wiphy_to_wifi(wiphy);
	return (struct esp32c3_private *)wifi->hw_priv;
}

/* ===================================================================
 * Hardware Operations Implementation
 * =================================================================== */

static int esp32c3_hw_init(struct wifi_core *wifi) {
	struct esp32c3_private *priv = (struct esp32c3_private *)wifi->hw_priv;
	char resp[256];
	int ret;

	printf("esp32c3: initializing module\n");

	/* Store reference to wifi_core */
	priv->wifi_core = wifi;

	/* Test AT communication */
	ret = wifi_core_send_cmd(wifi, "AT", resp, sizeof(resp), 1000);
	if (ret != 0) {
		printf("esp32c3: AT test failed\n");
		return ret;
	}

	/* Reset module if requested */
	if (priv->config.verbose) {
		printf("esp32c3: resetting module\n");
	}
	wifi_core_send_cmd(wifi, "AT+RST", resp, sizeof(resp), 1000);
	usleep(ESP32_RESET_DELAY_MS * 1000);

	/* Clear any boot messages */
	at_client_process_rx(wifi->at_client);

	/* Disable echo */
	ret = wifi_core_send_cmd(wifi, "ATE0", resp, sizeof(resp), 1000);
	if (ret != 0) {
		printf("esp32c3: failed to disable echo\n");
		return ret;
	}

	/* Set WiFi mode to STA */
	ret = wifi_core_send_cmd(wifi, "AT+CWMODE=1", resp, sizeof(resp), 2000);
	if (ret != 0) {
		printf("esp32c3: failed to set STA mode\n");
		return ret;
	}
	priv->wifi_mode = ESP32_WIFI_MODE_STA;

	/* Get firmware version */
	ret = wifi_core_send_cmd(wifi, "AT+GMR", resp, sizeof(resp), 2000);
	if (ret == 0) {
		if (strlen(resp) > 0) {
			strncpy(priv->status.fw_version, resp, 63);
			priv->status.fw_version[63] = '\0';
			printf("esp32c3: firmware %s\n", priv->status.fw_version);
		}
	}

	/* Get MAC address */
	ret = wifi_core_send_cmd(wifi, "AT+CIPSTAMAC?", resp, sizeof(resp), 2000);
	if (ret == 0) {
		char *mac_str = strstr(resp, "\"");
		if (mac_str) {
			mac_str++;
			unsigned int mac[6];
			if (sscanf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0],
			        &mac[1], &mac[2], &mac[3], &mac[4], &mac[5])
			    == 6) {
				for (int i = 0; i < 6; i++) {
					priv->status.mac_addr[i] = (uint8_t)mac[i];
				}
			}
		}
	}
	priv->initialized = true;
	priv->power_mode = ESP32_POWER_ACTIVE;

	printf("esp32c3: initialization complete\n");
	return 0;
}

static void esp32c3_hw_cleanup(struct wifi_core *wifi) {
	struct esp32c3_private *priv = (struct esp32c3_private *)wifi->hw_priv;

	if (priv->initialized) {
		/* Disconnect if connected */
		wifi_core_send_cmd(wifi, "AT+CWQAP", NULL, 0, 1000);
		priv->initialized = false;
	}
}

static int esp32c3_parse_scan_line(const char *line, struct wlan_scan_ap *ap) {
	const char *ptr;
	char *end;

	memset(ap, 0, sizeof(*ap));

	if (strncmp(line, "+CWLAP:", 7) != 0) {
		return -1;
	}

	ptr = line + 7;
	/* Skip '(' */
	if (*ptr++ != '(')
		return -1;

	/* Parse encryption type */
	ap->enc_type = strtol(ptr, &end, 10);
	if (end == ptr || *end != ',')
		return -1;
	ptr = end + 1;

	/* Parse SSID */
	if (*ptr++ != '"')
		return -1;
	char *ssid_end = strchr(ptr, '"');
	if (!ssid_end)
		return -1;
	size_t ssid_len = ssid_end - ptr;
	if (ssid_len > 32)
		ssid_len = 32;
	memcpy(ap->ssid, ptr, ssid_len);
	ap->ssid[ssid_len] = '\0';
	ptr = ssid_end + 1;

	/* Skip ',' */
	if (*ptr++ != ',')
		return -1;

	/* Parse RSSI */
	ap->rssi = (int8_t)strtol(ptr, &end, 10);
	if (end == ptr || *end != ',')
		return -1;
	ptr = end + 1;

	/* Parse BSSID */
	if (*ptr++ != '"')
		return -1;
	for (int i = 0; i < 6; i++) {
		ap->bssid[i] = (uint8_t)strtol(ptr, &end, 16);
		ptr = end;
		if (i < 5 && *ptr++ != ':')
			return -1;
	}
	if (*ptr++ != '"')
		return -1;
	if (*ptr++ != ',')
		return -1;

	/* Parse channel */
	ap->channel = (uint8_t)strtol(ptr, &end, 10);

	return 0;
}

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

static int esp32c3_get_ip_config(struct wifi_core *wifi, char *ip, char *mask,
    char *gw) {
	struct esp32c3_private *priv = (struct esp32c3_private *)wifi->hw_priv;
	char resp[512];
	int ret;

	ret = wifi_core_send_cmd(wifi, "AT+CIPSTA?", resp, sizeof(resp), 2000);
	if (ret != 0) {
		return ret;
	}

	/* Parse IP */
	char *p = strstr(resp, ":ip:");
	if (p) {
		char *start = strchr(p, '"');
		if (start) {
			start++;
			char *end = strchr(start, '"');
			if (end) {
				size_t len = end - start;
				if (len < 16) {
					memcpy(ip, start, len);
					ip[len] = '\0';
					strcpy(priv->status.ip_addr, ip);
				}
			}
		}
	}

	/* Parse Gateway */
	p = strstr(resp, ":gateway:");
	if (p) {
		char *start = strchr(p, '"');
		if (start) {
			start++;
			char *end = strchr(start, '"');
			if (end) {
				size_t len = end - start;
				if (len < 16) {
					memcpy(gw, start, len);
					gw[len] = '\0';
					strcpy(priv->status.gateway, gw);
				}
			}
		}
	}

	/* Parse Netmask */
	p = strstr(resp, ":netmask:");
	if (p) {
		char *start = strchr(p, '"');
		if (start) {
			start++;
			char *end = strchr(start, '"');
			if (end) {
				size_t len = end - start;
				if (len < 16) {
					memcpy(mask, start, len);
					mask[len] = '\0';
					strcpy(priv->status.netmask, mask);
				}
			}
		}
	}

	esp32c3_get_rssi(wifi, &wifi->conn_info.rssi);
	return 0;
}

static bool esp32c3_parse_urc(struct wifi_core *wifi, const char *line) {
	struct esp32c3_private *priv = (struct esp32c3_private *)wifi->hw_priv;

	/* Connection error */
	if (strstr(line, "+CWJAP:")) {
		int error_code;
		if (sscanf(line, "+CWJAP:%d", &error_code) == 1) {
			priv->status.last_error_code = error_code;
			int errno_val = esp32_error_to_errno(error_code);
			printf("esp32c3: connection error %d (errno %d)\n", error_code,
			    errno_val);
		}
		return true;
	}

	/* Disconnect reason */
	if (strstr(line, "WIFI DISCONNECT")) {
		int reason = 0;
		if (sscanf(line, "WIFI DISCONNECT,reason:%d", &reason) == 1) {
			priv->status.last_disconnect_reason = reason;
		}
		priv->status.connected = false;
		memset(priv->status.ssid, 0, sizeof(priv->status.ssid));
		memset(priv->status.ip_addr, 0, sizeof(priv->status.ip_addr));
		return true;
	}

	/* Connection success */
	if (strstr(line, "WIFI CONNECTED")) {
		priv->status.connected = true;
		/* Save current connection info */
		struct wifi_core *wifi_core = priv->wifi_core;
		if (wifi_core) {
			strcpy(priv->status.ssid, wifi_core->conn_info.ssid);
			priv->status.rssi = wifi_core->conn_info.rssi;
		}
		return true;
	}

	return false;
}

static int esp32c3_get_rssi(struct wifi_core *wifi, int8_t *rssi) {
	struct esp32c3_private *priv = (struct esp32c3_private *)wifi->hw_priv;
	char resp[256];
	int ret;

	ret = wifi_core_send_cmd(wifi, "AT+CWJAP?", resp, sizeof(resp), 2000);
	if (ret != 0) {
		return ret;
	}

	char *p = strstr(resp, "+CWJAP:");
	if (p) {
		/* Parse: +CWJAP:"ssid","mac",ch,rssi */
		int comma_count = 0;
		while (*p && comma_count < 3) {
			if (*p == ',')
				comma_count++;
			p++;
		}
		if (comma_count == 3) {
			int rssi_val = atoi(p);
			*rssi = rssi_val;
			priv->status.rssi = rssi_val;
			return 0;
		}
	}

	return -ENODATA;
}

static int esp32c3_parse_mode_response(const char *resp, int *mode) {
	char *p = strstr(resp, "+CWMODE:");
	if (p) {
		*mode = atoi(p + 8);
		return 0;
	}
	return -EINVAL;
}

static int esp32c3_hw_set_power_mode(struct wifi_core *wifi, int mode) {
	struct esp32c3_private *priv = (struct esp32c3_private *)wifi->hw_priv;
	char cmd[32];
	char resp[256];
	int sleep_mode;

	switch (mode) {
	case ESP32_POWER_ACTIVE:
		sleep_mode = 0;
		break;
	case ESP32_POWER_MODEM_SLEEP:
		sleep_mode = 1;
		break;
	case ESP32_POWER_LIGHT_SLEEP:
		sleep_mode = 2;
		break;
	default:
		return -EINVAL;
	}

	snprintf(cmd, sizeof(cmd), "AT+SLEEP=%d", sleep_mode);
	int ret = wifi_core_send_cmd(wifi, cmd, resp, sizeof(resp), 2000);

	if (ret == 0) {
		priv->power_mode = mode;
	}

	return ret;
}

static int esp32c3_hw_get_power_mode(struct wifi_core *wifi, int *mode) {
	struct esp32c3_private *priv = (struct esp32c3_private *)wifi->hw_priv;
	char resp[256];
	int ret;

	ret = wifi_core_send_cmd(wifi, "AT+SLEEP?", resp, sizeof(resp), 1000);
	if (ret == 0) {
		char *p = strstr(resp, "+SLEEP:");
		if (p) {
			int sleep_mode = atoi(p + 7);
			switch (sleep_mode) {
			case 0:
				*mode = ESP32_POWER_ACTIVE;
				break;
			case 1:
				*mode = ESP32_POWER_MODEM_SLEEP;
				break;
			case 2:
				*mode = ESP32_POWER_LIGHT_SLEEP;
				break;
			default:
				return -EINVAL;
			}
			priv->power_mode = *mode;
			return 0;
		}
	}

	return -EIO;
}

/* ===================================================================
 * Public API Implementation
 * =================================================================== */

struct wiphy *esp32c3_wifi_create(const char *name,
    const struct esp32c3_config *config) {
	struct wiphy *wiphy;
	struct esp32c3_private *priv;
	at_client_t *at_client = NULL;
	int fd = -1;
	int ret;

	/* Allocate private data */
	priv = sysmalloc(sizeof(struct esp32c3_private));
	if (!priv) {
		return NULL;
	}
	memset(priv, 0, sizeof(*priv));

	/* Copy configuration or use defaults */
	if (config) {
		memcpy(&priv->config, config, sizeof(struct esp32c3_config));
	}
	else {
		/* Set defaults */
		priv->config.connect_timeout = ESP32_DEFAULT_CONNECT_TIMEOUT_MS;
		priv->config.scan_timeout = ESP32_DEFAULT_SCAN_TIMEOUT_MS;
		priv->config.cmd_timeout = ESP32_DEFAULT_CMD_TIMEOUT_MS;
		priv->config.power_mode = ESP32_POWER_ACTIVE;
	}

	/* Open UART if path provided */
	if (priv->config.uart_path) {
		fd = open(priv->config.uart_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
		if (fd < 0) {
			printf("esp32c3: failed to open %s: %s\n", priv->config.uart_path,
			    strerror(errno));
			sysfree(priv);
			return NULL;
		}
		priv->owns_at_fd = true;
	}
	else {
		printf("esp32c3: no UART path provided\n");
		sysfree(priv);
		return NULL;
	}

	/* Create AT client */
	at_client = sysmalloc(sizeof(at_client_t));
	if (!at_client) {
		if (fd >= 0)
			close(fd);
		sysfree(priv);
		return NULL;
	}

	ret = at_client_init_fd(at_client, fd);
	if (ret != 0) {
		printf("esp32c3: failed to init AT client\n");
		sysfree(at_client);
		if (fd >= 0)
			close(fd);
		sysfree(priv);
		return NULL;
	}

	/* Configure AT client */
	at_client_set_mode(at_client, AT_CLIENT_MODE_NORMAL);
	if (priv->config.at_debug) {
		at_client_enable_debug(at_client, true);
	}

	priv->at_client = at_client;

	/* Create WiFi device with cfg80211 support */
	wiphy = wifi_core_create(name, at_client, &esp32c3_ops, priv);
	if (!wiphy) {
		printf("esp32c3: failed to create wifi device\n");
		at_client_close(at_client);
		sysfree(at_client);
		if (priv->owns_at_fd && fd >= 0)
			close(fd);
		sysfree(priv);
		return NULL;
	}

	printf("esp32c3: driver created\n");
	return wiphy;
}

struct wiphy *esp32c3_wifi_init_fd(const char *name, int fd) {
	struct esp32c3_config config = {.uart_path = NULL, /* Using provided fd */
	    .connect_timeout = ESP32_DEFAULT_CONNECT_TIMEOUT_MS,
	    .scan_timeout = ESP32_DEFAULT_SCAN_TIMEOUT_MS,
	    .cmd_timeout = ESP32_DEFAULT_CMD_TIMEOUT_MS,
	    .power_mode = ESP32_POWER_ACTIVE,
	    .auto_sleep = false,
	    .at_debug = false,
	    .verbose = false};

	struct wiphy *wiphy;
	struct esp32c3_private *priv;
	at_client_t *at_client;
	int ret;

	/* Allocate private data */
	priv = sysmalloc(sizeof(struct esp32c3_private));
	if (!priv) {
		return NULL;
	}
	memset(priv, 0, sizeof(*priv));
	memcpy(&priv->config, &config, sizeof(config));
	priv->owns_at_fd = false; /* We don't own this fd */

	/* Create AT client */
	at_client = sysmalloc(sizeof(at_client_t));
	if (!at_client) {
		sysfree(priv);
		return NULL;
	}

	ret = at_client_init_fd(at_client, fd);
	if (ret != 0) {
		sysfree(at_client);
		sysfree(priv);
		return NULL;
	}

	at_client_set_mode(at_client, AT_CLIENT_MODE_NORMAL);
	priv->at_client = at_client;

	/* Create WiFi device */
	wiphy = wifi_core_create(name, at_client, &esp32c3_ops, priv);
	if (!wiphy) {
		at_client_close(at_client);
		sysfree(at_client);
		sysfree(priv);
		return NULL;
	}

	return wiphy;
}

struct wiphy *esp32c3_wifi_init(const char *name, const char *uart_path) {
	struct esp32c3_config config = {.uart_path = uart_path,
	    .connect_timeout = ESP32_DEFAULT_CONNECT_TIMEOUT_MS,
	    .scan_timeout = ESP32_DEFAULT_SCAN_TIMEOUT_MS,
	    .cmd_timeout = ESP32_DEFAULT_CMD_TIMEOUT_MS,
	    .power_mode = ESP32_POWER_ACTIVE,
	    .auto_sleep = false,
	    .at_debug = false,
	    .verbose = false};

	return esp32c3_wifi_create(name, &config);
}

void esp32c3_wifi_cleanup(struct wiphy *wiphy) {
	struct wifi_core *wifi;
	struct esp32c3_private *priv;
	at_client_t *at_client;

	if (!wiphy) {
		return;
	}

	wifi = wiphy_to_wifi(wiphy);
	priv = (struct esp32c3_private *)wifi->hw_priv;
	at_client = priv->at_client;

	/* Destroy WiFi device */
	wifi_core_destroy(wiphy);

	/* Cleanup AT client */
	if (at_client) {
		at_client_close(at_client);
		sysfree(at_client);
	}

	/* Free private data */
	sysfree(priv);

	printf("esp32c3: driver cleaned up\n");
}

/* Status and control functions */
int esp32c3_get_status(struct wiphy *wiphy, struct esp32c3_status *status) {
	struct esp32c3_private *priv;
	struct wifi_core *wifi;

	if (!wiphy || !status) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	wifi = priv->wifi_core;

	if (!priv) {
		return -EINVAL;
	}

	if (wifi && wifi->wdev.conn_state == CFG80211_CONNECTED) {
		priv->status.connected = true;
		strcpy(priv->status.ssid, wifi->conn_info.ssid);
		strcpy(priv->status.ip_addr, wifi->conn_info.ip_addr);
		priv->status.rssi = wifi->conn_info.rssi;
	}
	else {
		priv->status.connected = false;
	}

	memcpy(status, &priv->status, sizeof(struct esp32c3_status));
	return 0;
}
int esp32c3_set_wifi_mode(struct wiphy *wiphy, int mode) {
	struct esp32c3_private *priv;
	struct wifi_core *wifi;
	char cmd[32];
	char resp[256];
	int ret;

	if (!wiphy) {
		return -EINVAL;
	}

	if (mode < ESP32_WIFI_MODE_NULL || mode > ESP32_WIFI_MODE_STA_AP) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	wifi = priv->wifi_core;

	snprintf(cmd, sizeof(cmd), "AT+CWMODE=%d", mode);
	ret = wifi_core_send_cmd(wifi, cmd, resp, sizeof(resp), 2000);

	if (ret == 0) {
		priv->wifi_mode = mode;
	}

	return ret;
}

int esp32c3_get_wifi_mode(struct wiphy *wiphy, int *mode) {
	struct esp32c3_private *priv;
	struct wifi_core *wifi;
	char resp[256];
	int ret;

	if (!wiphy || !mode) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	wifi = priv->wifi_core;

	ret = wifi_core_send_cmd(wifi, "AT+CWMODE?", resp, sizeof(resp), 1000);
	if (ret == 0) {
		ret = esp32c3_parse_mode_response(resp, mode);
		if (ret == 0) {
			priv->wifi_mode = *mode;
		}
	}

	return ret;
}

int esp32c3_set_power_mode(struct wiphy *wiphy, int mode) {
	struct esp32c3_private *priv;

	if (!wiphy) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	return esp32c3_hw_set_power_mode(priv->wifi_core, mode);
}

int esp32c3_get_power_mode(struct wiphy *wiphy, int *mode) {
	struct esp32c3_private *priv;

	if (!wiphy || !mode) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	return esp32c3_hw_get_power_mode(priv->wifi_core, mode);
}

int esp32c3_reset(struct wiphy *wiphy) {
	struct esp32c3_private *priv;
	struct wifi_core *wifi;
	char resp[256];

	if (!wiphy) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	wifi = priv->wifi_core;

	wifi_core_send_cmd(wifi, "AT+RST", resp, sizeof(resp), 1000);
	usleep(ESP32_RESET_DELAY_MS * 1000);

	/* Re-initialize after reset */
	return esp32c3_hw_init(wifi);
}

int esp32c3_get_firmware_version(struct wiphy *wiphy, char *version, size_t size) {
	struct esp32c3_private *priv;

	if (!wiphy || !version || size == 0) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	strncpy(version, priv->status.fw_version, size - 1);
	version[size - 1] = '\0';

	return 0;
}

int esp32c3_set_mac_address(struct wiphy *wiphy, const uint8_t *mac) {
	struct esp32c3_private *priv;
	struct wifi_core *wifi;
	char cmd[64];
	char resp[256];

	if (!wiphy || !mac) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	wifi = priv->wifi_core;

	snprintf(cmd, sizeof(cmd), "AT+CIPSTAMAC=\"%02x:%02x:%02x:%02x:%02x:%02x\"",
	    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return wifi_core_send_cmd(wifi, cmd, resp, sizeof(resp), 2000);
}

int esp32c3_get_mac_address(struct wiphy *wiphy, uint8_t *mac) {
	struct esp32c3_private *priv;

	if (!wiphy || !mac) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	memcpy(mac, priv->status.mac_addr, ESP32_MAC_ADDR_LEN);

	return 0;
}

int esp32c3_set_auto_connect(struct wiphy *wiphy, bool enable) {
	struct esp32c3_private *priv;
	struct wifi_core *wifi;
	char cmd[32];
	char resp[256];
	int ret;

	if (!wiphy) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	wifi = priv->wifi_core;

	snprintf(cmd, sizeof(cmd), "AT+CWAUTOCONN=%d", enable ? 1 : 0);
	ret = wifi_core_send_cmd(wifi, cmd, resp, sizeof(resp), 1000);

	if (ret == 0) {
		priv->auto_connect = enable;
	}

	return ret;
}

int esp32c3_set_dhcp_mode(struct wiphy *wiphy, bool enable) {
	struct esp32c3_private *priv;
	struct wifi_core *wifi;
	char cmd[32];
	char resp[256];
	int ret;

	if (!wiphy) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	wifi = priv->wifi_core;

	/* AT+CWDHCP=<mode>,<en> where mode=1 for STA */
	snprintf(cmd, sizeof(cmd), "AT+CWDHCP=1,%d", enable ? 1 : 0);
	ret = wifi_core_send_cmd(wifi, cmd, resp, sizeof(resp), 2000);

	if (ret == 0) {
		priv->dhcp_enabled = enable;
	}

	return ret;
}

int esp32c3_set_static_ip(struct wiphy *wiphy, const char *ip,
    const char *netmask, const char *gateway) {
	struct esp32c3_private *priv;
	struct wifi_core *wifi;
	char cmd[128];
	char resp[256];

	if (!wiphy || !ip) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	wifi = priv->wifi_core;

	/* First disable DHCP */
	esp32c3_set_dhcp_mode(wiphy, false);

	/* Set static IP */
	snprintf(cmd, sizeof(cmd), "AT+CIPSTA=\"%s\",\"%s\",\"%s\"", ip,
	    gateway ? gateway : "0.0.0.0", netmask ? netmask : "255.255.255.0");

	return wifi_core_send_cmd(wifi, cmd, resp, sizeof(resp), 2000);
}

int esp32c3_send_at_command(struct wiphy *wiphy, const char *cmd,
    char *response, size_t resp_size, uint32_t timeout_ms) {
	struct esp32c3_private *priv;
	struct wifi_core *wifi;

	if (!wiphy || !cmd) {
		return -EINVAL;
	}

	priv = wiphy_to_esp32(wiphy);
	wifi = priv->wifi_core;

	return wifi_core_send_cmd(wifi, cmd, response, resp_size, timeout_ms);
}

#if defined(CONFIG_ESP32C3_AUTO_INIT) && (CONFIG_ESP32C3_AUTO_INIT == 1)
static int esp32c3_wifi_init_unit(void) {
	const char *uart_path = OPTION_STRING_GET(uart_path);
	struct wiphy *wiphy;

	wiphy = esp32c3_wifi_init("wlan0", uart_path);
	return wiphy ? 0 : -ENOMEM;
}
EMBOX_UNIT_INIT(esp32c3_wifi_init_unit);
#endif