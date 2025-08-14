/**
 * @file
 * @brief WiFi Core Framework Implementation
 * 
 * @date July 31, 2025
 * @author Peize Li
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mem/sysmalloc.h>
#include <net/wlan/wireless_ioctl.h>
#include <wifi/wifi_core.h>

#define WIFI_SCAN_CACHE_DEFAULT 20
#define WIFI_RESP_BUFFER_SIZE   4096

/* Forward declarations */
static void wifi_core_urc_handler(const char *line, void *arg);
static void wifi_connect_timeout(struct sys_timer *timer, void *arg);
static int wifi_core_scan(struct net_device *dev);
static int wifi_core_connect(struct net_device *dev, const char *ssid,
    const char *pwd);
static int wifi_core_disconnect(struct net_device *dev);
static int wifi_core_set_mode(struct net_device *dev, int mode);
static int wifi_core_get_mode(struct net_device *dev, int *mode);
static int wifi_core_dev_init(struct net_device *dev);
static void wifi_core_dev_deinit(struct net_device *dev);
static int wifi_core_get_rssi(struct net_device *dev, int8_t *rssi);
static int wifi_core_set_power_mode(struct net_device *dev, int mode);

/* ===================================================================
 * Static Helper Functions
 * =================================================================== */

/**
 * @brief Set WiFi state with thread safety
 */
static void wifi_set_state(struct wifi_core *wifi, enum wifi_core_state state) {
	mutex_lock(&wifi->state_lock);
	wifi_core_debug(wifi, "state: %d -> %d", wifi->state, state);
	wifi->state = state;
	mutex_unlock(&wifi->state_lock);
}

/**
 * @brief Connection timeout handler
 */
static void wifi_connect_timeout(struct sys_timer *timer, void *arg) {
	struct wifi_core *wifi = (struct wifi_core *)arg;

	if (!wifi->timer_active) {
		return;
	}

	wifi->timer_active = false;

	if (!wifi->connect_done) {
		wifi_core_error(wifi, "connect timeout in state %d", wifi->state);
		wifi_set_state(wifi, WIFI_CORE_STATE_IDLE);
		wlan_device_set_state(wifi->netdev, WLAN_STATE_UP);

		mutex_lock(&wifi->conn_lock);
		wifi->conn_info.connected = false;
		memset(&wifi->conn_info, 0, sizeof(wifi->conn_info));
		mutex_unlock(&wifi->conn_lock);

		wifi->connect_result = -ETIMEDOUT;
		wifi->connect_done = true;

		wifi->stats.connect_failures++;
	}
}

/**
 * @brief URC (Unsolicited Result Code) handler
 */
static void wifi_core_urc_handler(const char *line, void *arg) {
	struct wifi_core *wifi = (struct wifi_core *)arg;
	const struct wifi_hw_ops *ops = wifi->hw_ops;

	if (wifi->config.debug_urc) {
		printf("wifi_core URC: %s\n", line);
	}

	/* Handle connect command response */
	if (wifi->state == WIFI_CORE_STATE_CONNECTING) {
		if (strstr(line, "OK") && !strstr(line, "+")) {
			wifi_set_state(wifi, WIFI_CORE_STATE_WAIT_CONNECTED);
			return;
		}
		if (strstr(line, "FAIL") || strstr(line, "ERROR")) {
			if (wifi->timer_active) {
				wifi->timer_active = false;
				timer_stop(&wifi->connect_timer);
			}
			wifi_set_state(wifi, WIFI_CORE_STATE_IDLE);
			wlan_device_set_state(wifi->netdev, WLAN_STATE_UP);
			wifi->stats.connect_failures++;

			wifi->connect_result = -ECONNREFUSED;
			wifi->connect_done = true;
			return;
		}
	}

	/* Handle connected URC */
	if (ops->urc_patterns.connected && strstr(line, ops->urc_patterns.connected)) {
		if (wifi->state == WIFI_CORE_STATE_WAIT_CONNECTED
		    || wifi->state == WIFI_CORE_STATE_CONNECTING) {
			wifi_set_state(wifi, WIFI_CORE_STATE_WAIT_IP);

			mutex_lock(&wifi->conn_lock);
			wifi->conn_info.connected = true;
			strcpy(wifi->conn_info.ssid, wifi->connect_req.ssid);
			mutex_unlock(&wifi->conn_lock);
		}
		return;
	}

	/* Handle got IP URC */
	if (ops->urc_patterns.got_ip && strstr(line, ops->urc_patterns.got_ip)) {
		if (wifi->state == WIFI_CORE_STATE_WAIT_IP) {
			if (wifi->timer_active) {
				wifi->timer_active = false;
				timer_stop(&wifi->connect_timer);
			}

			wifi_set_state(wifi, WIFI_CORE_STATE_CONNECTED);
			wlan_device_set_state(wifi->netdev, WLAN_STATE_CONNECTED);

			if (wifi->hw_ops->get_ip_config) {
				mutex_lock(&wifi->conn_lock);
				wifi->hw_ops->get_ip_config(wifi, wifi->conn_info.ip_addr,
				    wifi->conn_info.netmask, wifi->conn_info.gateway);
				wifi->conn_info.has_ip = true;
				mutex_unlock(&wifi->conn_lock);

				printf("wifi_core: connected to %s with IP %s\n",
				    wifi->conn_info.ssid, wifi->conn_info.ip_addr);
			}

			wifi->connect_result = 0;
			wifi->connect_done = true;

			wifi->stats.connect_attempts++;
		}
		return;
	}

	/* Handle disconnect URC */
	if (ops->urc_patterns.disconnected
	    && strstr(line, ops->urc_patterns.disconnected)) {
		if (wifi->timer_active) {
			wifi->timer_active = false;
			timer_stop(&wifi->connect_timer);
		}

		mutex_lock(&wifi->conn_lock);
		bool was_connected = wifi->conn_info.connected;
		wifi->conn_info.connected = false;
		wifi->conn_info.has_ip = false;
		mutex_unlock(&wifi->conn_lock);

		wifi->disconnect_done = true;

		if (!wifi->connect_done
		    && (wifi->state == WIFI_CORE_STATE_CONNECTING
		        || wifi->state == WIFI_CORE_STATE_WAIT_CONNECTED
		        || wifi->state == WIFI_CORE_STATE_WAIT_IP)) {
			wifi->connect_result = -ECONNABORTED;
			wifi->connect_done = true;
		}

		if (was_connected) {
			wifi_set_state(wifi, WIFI_CORE_STATE_IDLE);
			wlan_device_set_state(wifi->netdev, WLAN_STATE_UP);
			printf("wifi_core: disconnected\n");
		}
		return;
	}

	/* Let hardware specific handler process other URCs */
	if (wifi->hw_ops->parse_urc) {
		bool handled = wifi->hw_ops->parse_urc(wifi, line);
		if (handled) {
			wifi->stats.rx_packets++;
			return;
		}
	}

	if (wifi->config.debug_urc) {
		printf("wifi_core: unhandled URC: %s\n", line);
	}

	wifi->stats.rx_packets++;
}

/* ===================================================================
 * WiFi Operations Implementation
 * =================================================================== */

static int wifi_core_dev_init(struct net_device *dev) {
	/* Called by wlan_device_init after wifi_core instance is created */
	return 0;
}

static void wifi_core_dev_deinit(struct net_device *dev) {
	struct wlan_device *wdev = dev->wireless_priv;
	if (wdev && wdev->priv) {
		struct wifi_core *wifi = wdev->priv;
		wifi_core_cleanup(wifi);
	}
}

static int wifi_core_scan(struct net_device *dev) {
	struct wlan_device *wdev = dev->wireless_priv;
	struct wifi_core *wifi = wdev->priv;
	char *resp;
	const char *scan_cmd = NULL;
	int ret = -EINVAL;

	resp = sysmalloc(WIFI_RESP_BUFFER_SIZE);
	if (!resp) {
		return -ENOMEM;
	}

	wlan_device_set_state(dev, WLAN_STATE_SCANNING);
	wifi_set_state(wifi, WIFI_CORE_STATE_SCANNING);

	mutex_lock(&wifi->scan_lock);
	wifi->scan.count = 0;
	mutex_unlock(&wifi->scan_lock);

	if (wifi->hw_ops->get_scan_cmd) {
		scan_cmd = wifi->hw_ops->get_scan_cmd();
	}
	else if (wifi->hw_ops->basic_cmds.scan) {
		scan_cmd = wifi->hw_ops->basic_cmds.scan;
	}

	if (!scan_cmd) {
		wifi_core_error(wifi, "no scan command defined");
		ret = -EOPNOTSUPP;
	}
	else {
		ret = wifi_core_send_cmd(wifi, scan_cmd, resp, WIFI_RESP_BUFFER_SIZE,
		    wifi->hw_ops->timeouts.scan_ms);

		if (ret == 0) {
			char *line = strtok(resp, "\r\n");
			int line_count = 0;

			while (line && wifi->scan.count < wifi->scan.capacity) {
				printf("wifi_core_scan: line %d: '%s'\n", line_count++, line);

				struct wlan_scan_ap ap;
				if (wifi->hw_ops->parse_scan_line(line, &ap) == 0) {
					mutex_lock(&wifi->scan_lock);
					memcpy(&wifi->scan.aps[wifi->scan.count], &ap, sizeof(ap));
					wifi->scan.count++;
					mutex_unlock(&wifi->scan_lock);

					wifi_core_debug(wifi, "found AP: %s, rssi=%d", ap.ssid,
					    ap.rssi);
				}

				line = strtok(NULL, "\r\n");
			}

			mutex_lock(&wdev->lock);
			wdev->scan_result.aps = wifi->scan.aps;
			wdev->scan_result.count = wifi->scan.count;
			mutex_unlock(&wdev->lock);

			printf("wifi_core: scan complete, found %d APs\n", wifi->scan.count);
		}
	}

	if (wifi->state == WIFI_CORE_STATE_CONNECTED) {
		wlan_device_set_state(dev, WLAN_STATE_CONNECTED);
	}
	else {
		wifi_set_state(wifi, WIFI_CORE_STATE_IDLE);
		wlan_device_set_state(dev, WLAN_STATE_UP);
	}

	sysfree(resp);
	return ret;
}

static int wifi_core_connect(struct net_device *dev, const char *ssid,
    const char *pwd) {
	struct wlan_device *wdev = dev->wireless_priv;
	struct wifi_core *wifi = wdev->priv;
	char cmd[256];
	int ret;
	int retry_count = 0;

	if (!ssid || strlen(ssid) > 32) {
		return -EINVAL;
	}

	/* Disconnect if already connected */
	if (wifi->conn_info.connected) {
		wifi->disconnect_done = false;
		wifi_core_disconnect(dev);

		/* Wait for disconnect completion (max 3 seconds) */
		for (retry_count = 0; retry_count < 30 && !wifi->disconnect_done;
		     retry_count++) {
			at_client_process_rx(wifi->at_client);
			usleep(100000); /* 100ms */
		}

		if (!wifi->disconnect_done) {
			printf("wifi_core: disconnect timeout\n");
		}
	}

	/* Save connection parameters */
	mutex_lock(&wifi->conn_lock);
	strncpy(wifi->connect_req.ssid, ssid, 32);
	wifi->connect_req.ssid[32] = '\0';
	if (pwd) {
		strncpy(wifi->connect_req.password, pwd, 64);
		wifi->connect_req.password[64] = '\0';
		wifi->connect_req.has_password = true;
	}
	else {
		wifi->connect_req.has_password = false;
	}
	mutex_unlock(&wifi->conn_lock);

	/* Build connect command */
	ret = wifi->hw_ops->build_connect_cmd(cmd, sizeof(cmd), ssid, pwd);
	if (ret != 0) {
		return -EINVAL;
	}

	wifi->connect_done = false;
	wifi->connect_result = -EINPROGRESS;

	wifi_set_state(wifi, WIFI_CORE_STATE_CONNECTING);

	/* Start timeout timer */
	wifi->timer_active = true;
	timer_init_start_msec(&wifi->connect_timer, TIMER_ONESHOT,
	    wifi->hw_ops->timeouts.connect_ms, wifi_connect_timeout, wifi);

	/* Send connect command */
	at_result_t result = at_client_send(wifi->at_client, cmd, NULL, 0, 1000);

	if (result != AT_OK) {
		wifi->timer_active = false;
		timer_stop(&wifi->connect_timer);
		wifi_set_state(wifi, WIFI_CORE_STATE_IDLE);
		wlan_device_set_state(dev, WLAN_STATE_UP);

		printf("wifi_core: failed to send connect command\n");
		return -EIO;
	}

	/* Wait for connection completion */
	uint32_t wait_ms = 0;
	while (!wifi->connect_done && wait_ms < wifi->hw_ops->timeouts.connect_ms) {
		at_client_process_rx(wifi->at_client);
		usleep(50000); /* 50ms */
		wait_ms += 50;
	}

	if (wifi->timer_active) {
		wifi->timer_active = false;
		timer_stop(&wifi->connect_timer);
	}

	if (wifi->connect_done) {
		return wifi->connect_result;
	}
	else {
		wifi_set_state(wifi, WIFI_CORE_STATE_IDLE);
		wlan_device_set_state(dev, WLAN_STATE_UP);
		return -ETIMEDOUT;
	}
}

static int wifi_core_disconnect(struct net_device *dev) {
	struct wlan_device *wdev = dev->wireless_priv;
	struct wifi_core *wifi = wdev->priv;
	const char *disconnect_cmd;
	char resp[256];
	int ret;

	timer_stop(&wifi->connect_timer);

	if (wifi->hw_ops->get_disconnect_cmd) {
		disconnect_cmd = wifi->hw_ops->get_disconnect_cmd();
	}
	else if (wifi->hw_ops->basic_cmds.disconnect) {
		disconnect_cmd = wifi->hw_ops->basic_cmds.disconnect;
	}
	else {
		wifi_core_error(wifi, "no disconnect command defined");
		return -EOPNOTSUPP;
	}

	ret = wifi_core_send_cmd(wifi, disconnect_cmd, resp, sizeof(resp),
	    wifi->hw_ops->timeouts.cmd_ms);

	mutex_lock(&wifi->conn_lock);
	wifi->conn_info.connected = false;
	wifi->conn_info.has_ip = false;
	memset(&wifi->conn_info, 0, sizeof(wifi->conn_info));
	mutex_unlock(&wifi->conn_lock);

	wifi_set_state(wifi, WIFI_CORE_STATE_IDLE);
	wlan_device_set_state(dev, WLAN_STATE_UP);

	mutex_lock(&wdev->lock);
	memset(&wdev->connected, 0, sizeof(wdev->connected));
	mutex_unlock(&wdev->lock);

	if (ret == 0) {
		printf("wifi_core: disconnected\n");
	}

	return ret;
}

static int wifi_core_get_rssi(struct net_device *dev, int8_t *rssi) {
	struct wlan_device *wdev = dev->wireless_priv;
	struct wifi_core *wifi = wdev->priv;

	if (!wifi->conn_info.connected) {
		return -ENOTCONN;
	}

	if (wifi->hw_ops->get_rssi) {
		return wifi->hw_ops->get_rssi(wifi, rssi);
	}

	return -EOPNOTSUPP;
}

static int wifi_core_set_power_mode(struct net_device *dev, int mode) {
	struct wlan_device *wdev = dev->wireless_priv;
	struct wifi_core *wifi = wdev->priv;

	if (wifi->hw_ops->set_power_mode) {
		return wifi->hw_ops->set_power_mode(wifi, mode);
	}

	return -EOPNOTSUPP;
}

static int wifi_core_get_power_mode(struct net_device *dev, int *mode) {
	struct wlan_device *wdev = dev->wireless_priv;
	struct wifi_core *wifi = wdev->priv;

	if (wifi->hw_ops->get_power_mode) {
		return wifi->hw_ops->get_power_mode(wifi, mode);
	}

	return -EOPNOTSUPP;
}

static int wifi_core_set_mode(struct net_device *dev, int mode) {
	struct wlan_device *wdev = dev->wireless_priv;
	struct wifi_core *wifi = wdev->priv;
	char cmd[64];
	char resp[256];

	if (!wifi->hw_ops->mode_cmds.set_mode_fmt) {
		return -EOPNOTSUPP;
	}

	snprintf(cmd, sizeof(cmd), wifi->hw_ops->mode_cmds.set_mode_fmt, mode);
	return wifi_core_send_cmd(wifi, cmd, resp, sizeof(resp), 1000);
}

static int wifi_core_get_mode(struct net_device *dev, int *mode) {
	struct wlan_device *wdev = dev->wireless_priv;
	struct wifi_core *wifi = wdev->priv;
	char resp[256];

	if (!wifi->hw_ops->mode_cmds.get_mode) {
		return -EOPNOTSUPP;
	}

	int ret = wifi_core_send_cmd(wifi, wifi->hw_ops->mode_cmds.get_mode, resp,
	    sizeof(resp), 1000);
	if (ret == 0) {
		if (wifi->hw_ops->parse_mode_response) {
			return wifi->hw_ops->parse_mode_response(resp, mode);
		}
	}
	return -EIO;
}

/* ===================================================================
 * Public API Functions
 * =================================================================== */

int wifi_core_setup(struct wifi_core *wifi, struct net_device *netdev,
    at_client_t *at_client, const struct wifi_hw_ops *hw_ops, void *hw_priv) {
	int ret;

	if (!wifi || !netdev || !at_client || !hw_ops) {
		return -EINVAL;
	}

	if (!hw_ops->hw_init || !hw_ops->parse_scan_line || !hw_ops->build_connect_cmd) {
		printf("wifi_core: missing required operations\n");
		return -EINVAL;
	}

	memset(wifi, 0, sizeof(*wifi));

	wifi->netdev = netdev;
	wifi->at_client = at_client;
	wifi->hw_ops = hw_ops;
	wifi->hw_priv = hw_priv;
	wifi->connect_done = true;
	wifi->disconnect_done = true;
	wifi->timer_active = false;
	wifi->connect_result = 0;

	mutex_init(&wifi->state_lock);
	mutex_init(&wifi->conn_lock);
	mutex_init(&wifi->scan_lock);

	wifi->state = WIFI_CORE_STATE_UNINIT;

	wifi->config.auto_reconnect = false;
	wifi->config.retry_count = 3;
	wifi->config.retry_delay_ms = 1000;

	wifi->scan.capacity = WIFI_SCAN_CACHE_DEFAULT;
	wifi->scan.aps = sysmalloc(wifi->scan.capacity * sizeof(struct wlan_scan_ap));
	if (!wifi->scan.aps) {
		return -ENOMEM;
	}

	at_client_set_urc_handler(at_client, wifi_core_urc_handler, wifi);

	if (netdev->wireless_priv) {
		struct wlan_device *wdev = netdev->wireless_priv;
		wdev->priv = wifi;
	}

	ret = hw_ops->hw_init(wifi);
	if (ret != 0) {
		printf("wifi_core: hardware init failed: %d\n", ret);
		sysfree(wifi->scan.aps);
		return ret;
	}

	wifi_set_state(wifi, WIFI_CORE_STATE_IDLE);
	wlan_device_set_state(netdev, WLAN_STATE_UP);

	printf("wifi_core: initialized for %s\n", netdev->name);
	return 0;
}

void wifi_core_cleanup(struct wifi_core *wifi) {
	if (!wifi) {
		return;
	}

	if (wifi->timer_active) {
		wifi->timer_active = false;
		timer_stop(&wifi->connect_timer);
	}

	/* Only disconnect if connected */
	if (wifi->state == WIFI_CORE_STATE_CONNECTED && wifi->conn_info.connected) {
		const char *disconnect_cmd = NULL;

		if (wifi->hw_ops->get_disconnect_cmd) {
			disconnect_cmd = wifi->hw_ops->get_disconnect_cmd();
		}
		else if (wifi->hw_ops->basic_cmds.disconnect) {
			disconnect_cmd = wifi->hw_ops->basic_cmds.disconnect;
		}

		if (disconnect_cmd) {
			char resp[64];
			wifi_core_send_cmd(wifi, disconnect_cmd, resp, sizeof(resp), 500);
		}
		else {
			wifi_core_error(wifi, "no disconnect command defined, skipping "
			                      "disconnect");
		}
	}

	if (wifi->hw_ops->hw_cleanup) {
		wifi->hw_ops->hw_cleanup(wifi);
	}

	at_client_set_urc_handler(wifi->at_client, NULL, NULL);

	mutex_lock(&wifi->scan_lock);
	if (wifi->scan.aps) {
		sysfree(wifi->scan.aps);
		wifi->scan.aps = NULL;
	}
	mutex_unlock(&wifi->scan_lock);

	wifi_set_state(wifi, WIFI_CORE_STATE_UNINIT);
	wlan_device_set_state(wifi->netdev, WLAN_STATE_DOWN);

	printf("wifi_core: cleaned up\n");
}

int wifi_core_send_cmd(struct wifi_core *wifi, const char *cmd, char *resp,
    size_t resp_size, uint32_t timeout_ms) {
	at_result_t result;

	if (timeout_ms == 0) {
		timeout_ms = wifi->hw_ops->timeouts.cmd_ms;
	}

	wifi_core_debug(wifi, "sending: %s", cmd);

	result = at_client_send(wifi->at_client, cmd, resp, resp_size, timeout_ms);

	if (result != AT_OK) {
		wifi_core_debug(wifi, "command failed: %d", result);
		wifi->stats.tx_errors++;
		return -EIO;
	}

	if (resp && resp_size > 0) {
		int resp_len = strlen(resp);
		printf("wifi_core: received %d bytes response\n", resp_len);
		if (resp_len > 0) {
			char debug_buf[101];
			strncpy(debug_buf, resp, 100);
			debug_buf[100] = '\0';
			printf("wifi_core: response preview: %s%s\n", debug_buf,
			    resp_len > 100 ? "..." : "");
		}
	}

	wifi->stats.tx_packets++;
	return 0;
}

void wifi_core_set_auto_reconnect(struct wifi_core *wifi, bool enable) {
	if (wifi) {
		wifi->config.auto_reconnect = enable;
	}
}

void wifi_core_set_retry_params(struct wifi_core *wifi, int count,
    uint32_t delay_ms) {
	if (wifi) {
		wifi->config.retry_count = count;
		wifi->config.retry_delay_ms = delay_ms;
	}
}

void wifi_core_set_debug_urc(struct wifi_core *wifi, bool enable) {
	if (wifi) {
		wifi->config.debug_urc = enable;
	}
}

bool wifi_core_is_connected(struct wifi_core *wifi) {
	bool connected;

	if (!wifi) {
		return false;
	}

	mutex_lock(&wifi->conn_lock);
	connected = wifi->conn_info.connected;
	mutex_unlock(&wifi->conn_lock);

	return connected;
}

bool wifi_core_has_ip(struct wifi_core *wifi) {
	bool has_ip;

	if (!wifi) {
		return false;
	}

	mutex_lock(&wifi->conn_lock);
	has_ip = wifi->conn_info.has_ip;
	mutex_unlock(&wifi->conn_lock);

	return has_ip;
}

int wifi_core_get_connection_info(struct wifi_core *wifi, char *ssid, char *ip,
    int8_t *rssi) {
	if (!wifi || !wifi->conn_info.connected) {
		return -ENOTCONN;
	}

	mutex_lock(&wifi->conn_lock);

	if (ssid) {
		strcpy(ssid, wifi->conn_info.ssid);
	}
	if (ip) {
		strcpy(ip, wifi->conn_info.ip_addr);
	}
	if (rssi) {
		/* Try to get real-time RSSI value */
		if (wifi->hw_ops->get_rssi) {
			if (wifi->hw_ops->get_rssi(wifi, rssi) != 0) {
				*rssi = wifi->conn_info.rssi;
			}
		}
		else {
			*rssi = wifi->conn_info.rssi;
		}
	}

	mutex_unlock(&wifi->conn_lock);

	return 0;
}

/* ===================================================================
 * WiFi Core Operations Structure
 * =================================================================== */

const struct wireless_ops wifi_core_ops = {
    .init = wifi_core_dev_init,
    .deinit = wifi_core_dev_deinit,
    .scan = wifi_core_scan,
    .connect = wifi_core_connect,
    .disconnect = wifi_core_disconnect,
    .get_rssi = wifi_core_get_rssi,
    .set_power_mode = wifi_core_set_power_mode,
    .get_power_mode = wifi_core_get_power_mode,
    .set_mode = wifi_core_set_mode,
    .get_mode = wifi_core_get_mode,
};
