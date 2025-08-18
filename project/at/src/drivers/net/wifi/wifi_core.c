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

#include <hal/clock.h>
#include <mem/sysmalloc.h>
#include <net/inetdevice.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <wifi/wifi_core.h>

/* Temporary network device allocation */
#define NET_NAME_UNKNOWN ""
#define ether_setup      NULL

#define WIFI_SCAN_CACHE_DEFAULT 20
#define WIFI_RESP_BUFFER_SIZE   4096

static inline struct net_device *alloc_netdev(int priv_size, const char *name,
    const char *name_assign_type, void (*setup)(struct net_device *)) {
	struct net_device *dev = sysmalloc(sizeof(struct net_device) + priv_size);
	if (dev) {
		memset(dev, 0, sizeof(struct net_device) + priv_size);
		strncpy(dev->name, name, IFNAMSIZ - 1);
		if (priv_size > 0) {
			dev->priv = (void *)((char *)dev + sizeof(struct net_device));
		}
	}
	return dev;
}

static inline void free_netdev(struct net_device *dev) {
	if (dev) {
		inetdev_unregister_dev(dev);
		sysfree(dev);
	}
}

/* Forward declarations */
static void wifi_core_urc_handler(const char *line, void *arg);
static void wifi_connect_timeout(struct sys_timer *timer, void *arg);

/* ===================================================================
 * cfg80211 Operations Implementation
 * =================================================================== */

/**
 * cfg80211 connect operation - initiates connection to an access point
 */
static int wifi_cfg80211_connect(struct wiphy *wiphy, struct net_device *dev,
    struct cfg80211_connect_params *sme) {
	struct wifi_core *wifi = wiphy_to_wifi(wiphy);
	char cmd[256];
	int ret;

	if (!sme->ssid || sme->ssid_len == 0 || sme->ssid_len > 32) {
		return -EINVAL;
	}

	/* Check if already connected */
	if (wifi->wdev.conn_state == CFG80211_CONNECTED) {
		return -EALREADY;
	}

	/* Save connection parameters */
	mutex_lock(&wifi->conn_lock);
	memcpy(wifi->conn_info.ssid, sme->ssid, sme->ssid_len);
	wifi->conn_info.ssid[sme->ssid_len] = '\0';
	mutex_unlock(&wifi->conn_lock);

	/* Build connect command */
	ret = wifi->hw_ops->build_connect_cmd(cmd, sizeof(cmd),
	    (const char *)sme->ssid, (const char *)sme->key);
	if (ret != 0) {
		return -EINVAL;
	}

	/* Update state to connecting */
	cfg80211_update_connection_state(&wifi->wdev, CFG80211_CONNECTING);

	/* Start connection timer */
	wifi->timer_active = true;
	timer_init_start_msec(&wifi->connect_timer, TIMER_ONESHOT,
	    wifi->hw_ops->timeouts.connect_ms, wifi_connect_timeout, wifi);

	/* Send connect command */
	at_result_t result = at_client_send(wifi->at_client, cmd, NULL, 0, 1000);

	if (result != AT_OK) {
		wifi->timer_active = false;
		timer_stop(&wifi->connect_timer);
		cfg80211_update_connection_state(&wifi->wdev, CFG80211_DISCONNECTED);
		wifi->stats.connect_failures++;

		wifi_core_error(wifi, "failed to send connect command");
		return -EIO;
	}

	wifi->stats.connect_attempts++;
	wifi_core_debug(wifi, "connecting to %s", wifi->conn_info.ssid);

	/* Connection will complete asynchronously via URC */
	return 0;
}

/**
 * cfg80211 disconnect operation
 */
static int wifi_cfg80211_disconnect(struct wiphy *wiphy, struct net_device *dev,
    uint16_t reason_code) {
	struct wifi_core *wifi = wiphy_to_wifi(wiphy);
	const char *disconnect_cmd;
	char resp[256];
	int ret;

	/* Stop any pending connection timer */
	if (wifi->timer_active) {
		wifi->timer_active = false;
		timer_stop(&wifi->connect_timer);
	}

	/* Get disconnect command */
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

	/* Send disconnect command */
	ret = wifi_core_send_cmd(wifi, disconnect_cmd, resp, sizeof(resp),
	    wifi->hw_ops->timeouts.cmd_ms);

	/* Clear connection info */
	mutex_lock(&wifi->conn_lock);
	memset(&wifi->conn_info, 0, sizeof(wifi->conn_info));
	mutex_unlock(&wifi->conn_lock);

	/* Update state */
	cfg80211_update_connection_state(&wifi->wdev, CFG80211_DISCONNECTED);

	if (ret == 0) {
		wifi_core_debug(wifi, "disconnected");
	}

	return ret;
}

/**
 * cfg80211 scan operation - performs AP scan and populates BSS list
 */
static int wifi_cfg80211_scan(struct wiphy *wiphy,
    struct cfg80211_scan_request *request) {
	struct wifi_core *wifi = wiphy_to_wifi(wiphy);
	char *resp;
	const char *scan_cmd = NULL;
	int ret = -EINVAL;

	/* Check if already scanning */
	if (wifi->wdev.scanning) {
		return -EBUSY;
	}

	wifi->wdev.scanning = true;

	/* Allocate response buffer */
	resp = sysmalloc(WIFI_RESP_BUFFER_SIZE);
	if (!resp) {
		wifi->wdev.scanning = false;
		return -ENOMEM;
	}

	/* Clear previous scan results in cfg80211 */
	cfg80211_clear_scan_results(&wifi->wdev);

	/* Clear local scan cache */
	mutex_lock(&wifi->scan_lock);
	wifi->scan.count = 0;
	mutex_unlock(&wifi->scan_lock);

	/* Get scan command */
	if (wifi->hw_ops->get_scan_cmd) {
		scan_cmd = wifi->hw_ops->get_scan_cmd();
	}
	else if (wifi->hw_ops->basic_cmds.scan) {
		scan_cmd = wifi->hw_ops->basic_cmds.scan;
	}

	if (!scan_cmd) {
		wifi_core_error(wifi, "no scan command defined");
		ret = -EOPNOTSUPP;
		goto scan_done;
	}

	/* Send scan command */
	ret = wifi_core_send_cmd(wifi, scan_cmd, resp, WIFI_RESP_BUFFER_SIZE,
	    wifi->hw_ops->timeouts.scan_ms);

	if (ret == 0) {
		/* Parse scan results and add to cfg80211 */
		char *line = strtok(resp, "\r\n");
		int ap_count = 0;
		int line_count __attribute__((unused)) = 0;

		while (line) {
			struct wlan_scan_ap ap;

			/* Debug output - show raw scan response */
			wifi_core_debug(wifi, "scan line %d: '%s'", line_count++, line);

			if (wifi->hw_ops->parse_scan_line(line, &ap) == 0) {
				/* Add to cfg80211 BSS list */
				cfg80211_add_bss(&wifi->wdev, ap.bssid, (const uint8_t *)ap.ssid,
				    strlen(ap.ssid), ap.rssi, ap.channel, ap.enc_type);

				/* Also save in local cache for debugging */
				mutex_lock(&wifi->scan_lock);
				if (wifi->scan.count < wifi->scan.capacity) {
					memcpy(&wifi->scan.aps[wifi->scan.count], &ap, sizeof(ap));
					wifi->scan.count++;
				}
				mutex_unlock(&wifi->scan_lock);

				ap_count++;
				wifi_core_debug(wifi, "found AP: %s, rssi=%d, ch=%d, enc=%d",
				    ap.ssid, ap.rssi, ap.channel, ap.enc_type);
			}

			line = strtok(NULL, "\r\n");
		}

		wifi_core_debug(wifi, "scan complete, found %d APs", ap_count);
	}
	else {
		wifi_core_error(wifi, "scan command failed with result %d", ret);
	}

scan_done:
	sysfree(resp);

	/* Mark scan as complete */
	wifi->wdev.scanning = false;
	wifi->wdev.last_scan_complete = clock_sys_ticks();
	cfg80211_scan_done(&wifi->wdev, ret != 0);

	return ret;
}

static const struct cfg80211_ops wifi_cfg80211_ops = {
    .connect = wifi_cfg80211_connect,
    .disconnect = wifi_cfg80211_disconnect,
    .scan = wifi_cfg80211_scan,
};

/* ===================================================================
 * Internal Helper Functions
 * =================================================================== */

/**
 * Connection timeout handler - called when connection attempt times out
 */
static void wifi_connect_timeout(struct sys_timer *timer, void *arg) {
	struct wifi_core *wifi = (struct wifi_core *)arg;

	if (!wifi->timer_active) {
		return;
	}

	wifi->timer_active = false;

	wifi_core_error(wifi, "connection timeout");

	/* Update state */
	cfg80211_update_connection_state(&wifi->wdev, CFG80211_DISCONNECTED);

	/* Notify cfg80211 of connection failure */
	cfg80211_connect_result(wifi->netdev, NULL, NULL, 0, NULL, 0,
	    WLAN_STATUS_UNSPECIFIED_FAILURE, GFP_KERNEL);

	/* Clear connection info */
	mutex_lock(&wifi->conn_lock);
	memset(&wifi->conn_info, 0, sizeof(wifi->conn_info));
	mutex_unlock(&wifi->conn_lock);

	wifi->stats.connect_failures++;
}

/**
 * URC (Unsolicited Result Code) handler - processes async messages from WiFi module
 */
static void wifi_core_urc_handler(const char *line, void *arg) {
	struct wifi_core *wifi = (struct wifi_core *)arg;
	const struct wifi_hw_ops *ops = wifi->hw_ops;

	/* Output all URCs for debugging */
	wifi_core_debug(wifi, "URC: %s", line);

	/* Handle connection success */
	if (ops->urc_patterns.connected && strstr(line, ops->urc_patterns.connected)) {
		if (wifi->wdev.conn_state == CFG80211_CONNECTING) {
			cfg80211_update_connection_state(&wifi->wdev, CFG80211_CONNECTED);
			wifi_core_debug(wifi, "WiFi connected");
		}
		return;
	}

	/* Handle IP acquisition */
	if (ops->urc_patterns.got_ip && strstr(line, ops->urc_patterns.got_ip)) {
		if (wifi->timer_active) {
			wifi->timer_active = false;
			timer_stop(&wifi->connect_timer);
		}

		/* Get IP configuration if available */
		if (wifi->hw_ops->get_ip_config) {
			mutex_lock(&wifi->conn_lock);
			wifi->hw_ops->get_ip_config(wifi, wifi->conn_info.ip_addr,
			    wifi->conn_info.netmask, wifi->conn_info.gateway);
			mutex_unlock(&wifi->conn_lock);

			wifi_core_debug(wifi, "got IP: %s", wifi->conn_info.ip_addr);
		}

		/* Notify cfg80211 of successful connection */
		cfg80211_connect_result(wifi->netdev, NULL, NULL, 0, NULL, 0,
		    WLAN_STATUS_SUCCESS, GFP_KERNEL);

		wifi_core_debug(wifi, "connected to %s with IP %s",
		    wifi->conn_info.ssid, wifi->conn_info.ip_addr);
		return;
	}

	/* Handle disconnection */
	if (ops->urc_patterns.disconnected
	    && strstr(line, ops->urc_patterns.disconnected)) {
		if (wifi->timer_active) {
			wifi->timer_active = false;
			timer_stop(&wifi->connect_timer);
		}

		/* Clear connection info */
		mutex_lock(&wifi->conn_lock);
		bool was_connected = (wifi->wdev.conn_state == CFG80211_CONNECTED);
		memset(&wifi->conn_info, 0, sizeof(wifi->conn_info));
		mutex_unlock(&wifi->conn_lock);

		/* Update state */
		cfg80211_update_connection_state(&wifi->wdev, CFG80211_DISCONNECTED);

		/* Notify cfg80211 if we were connected */
		if (was_connected) {
			cfg80211_disconnected(wifi->netdev, WLAN_REASON_DEAUTH_LEAVING,
			    NULL, 0, true, GFP_KERNEL);
			wifi_core_debug(wifi, "WiFi disconnected");
		}
		return;
	}

	/* Let hardware specific handler process other URCs */
	if (wifi->hw_ops->parse_urc) {
		if (wifi->hw_ops->parse_urc(wifi, line)) {
			wifi_core_debug(wifi, "URC handled by hw_ops");
		}
		else {
			wifi_core_debug(wifi, "unhandled URC: %s", line);
		}
	}

	wifi->stats.rx_packets++;
}

/* ===================================================================
 * Public API Functions
 * =================================================================== */

struct wiphy *wifi_core_create(const char *name, at_client_t *at_client,
    const struct wifi_hw_ops *hw_ops, void *hw_priv) {
	struct wiphy *wiphy;
	struct wifi_core *wifi;
	struct net_device *netdev;
	int ret;

	/* Validate parameters */
	if (!name || !at_client || !hw_ops) {
		return NULL;
	}

	if (!hw_ops->hw_init || !hw_ops->parse_scan_line || !hw_ops->build_connect_cmd) {
		wifi_core_error(NULL, "missing required operations");
		return NULL;
	}

	/* Allocate wiphy with embedded wifi_core */
	wiphy = wiphy_new_nm(&wifi_cfg80211_ops, sizeof(struct wifi_core), name);
	if (!wiphy) {
		return NULL;
	}

	wifi = wiphy_priv(wiphy);
	memset(wifi, 0, sizeof(*wifi));

	/* Initialize basic members */
	wifi->wiphy = wiphy;
	wifi->at_client = at_client;
	wifi->hw_ops = hw_ops;
	wifi->hw_priv = hw_priv;

	/* Initialize mutexes */
	mutex_init(&wifi->state_lock);
	mutex_init(&wifi->conn_lock);
	mutex_init(&wifi->scan_lock);

	/* Allocate scan cache */
	wifi->scan.capacity = WIFI_SCAN_CACHE_DEFAULT;
	wifi->scan.aps = sysmalloc(wifi->scan.capacity * sizeof(struct wlan_scan_ap));
	if (!wifi->scan.aps) {
		wiphy_free(wiphy);
		return NULL;
	}

	/* Create network device */
	// netdev = alloc_netdev(0, name, NET_NAME_UNKNOWN, ether_setup);
	netdev = etherdev_alloc(0);
	if (!netdev) {
		sysfree(wifi->scan.aps);
		wiphy_free(wiphy);
		return NULL;
	}

	strncpy(netdev->name, name, IFNAMSIZ - 1);

	/* Setup wireless_dev */
	wifi->wdev.wiphy = wiphy;
	wifi->wdev.netdev = netdev;
	netdev->nd_ieee80211_ptr = &wifi->wdev;
	wifi->netdev = netdev;

	/* Register URC handler */
	at_client_set_urc_handler(at_client, wifi_core_urc_handler, wifi);

	/* Initialize hardware */
	if (hw_ops->hw_init) {
		ret = hw_ops->hw_init(wifi);
		if (ret != 0) {
			wifi_core_error(wifi, "hardware init failed: %d", ret);
			at_client_set_urc_handler(at_client, NULL, NULL);
			sysfree(wifi->scan.aps);
			free_netdev(netdev);
			wiphy_free(wiphy);
			return NULL;
		}
	}

	ret = inetdev_register_dev(netdev);
	if (ret != 0) {
		wifi_core_error(wifi, "inetdev registration failed: %d", ret);
		if (hw_ops->hw_cleanup) {
			hw_ops->hw_cleanup(wifi);
		}
		at_client_set_urc_handler(at_client, NULL, NULL);
		sysfree(wifi->scan.aps);
		free_netdev(netdev);
		wiphy_free(wiphy);
		return NULL;
	}

	/* Register with cfg80211 */
	ret = cfg80211_register_netdevice(netdev);
	if (ret != 0) {
		wifi_core_error(wifi, "cfg80211 registration failed: %d", ret);
		if (hw_ops->hw_cleanup) {
			hw_ops->hw_cleanup(wifi);
		}
		at_client_set_urc_handler(at_client, NULL, NULL);
		sysfree(wifi->scan.aps);
		free_netdev(netdev);
		wiphy_free(wiphy);
		return NULL;
	}

	wifi_core_debug(wifi, "created %s with cfg80211 support", name);
	return wiphy;
}

void wifi_core_destroy(struct wiphy *wiphy) {
	struct wifi_core *wifi;

	if (!wiphy) {
		return;
	}

	wifi = wiphy_to_wifi(wiphy);

	/* Stop any active timer */
	if (wifi->timer_active) {
		wifi->timer_active = false;
		timer_stop(&wifi->connect_timer);
	}

	/* Disconnect if connected */
	if (wifi->wdev.conn_state == CFG80211_CONNECTED) {
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
	}

	/* Unregister from cfg80211 */
	cfg80211_unregister_netdevice(wifi->netdev);

	/* Hardware cleanup */
	if (wifi->hw_ops->hw_cleanup) {
		wifi->hw_ops->hw_cleanup(wifi);
	}

	/* Clear URC handler */
	at_client_set_urc_handler(wifi->at_client, NULL, NULL);

	/* Free scan cache */
	if (wifi->scan.aps) {
		sysfree(wifi->scan.aps);
	}

	/* Free network device and wiphy */
	free_netdev(wifi->netdev);
	wiphy_free(wiphy);
}

int wifi_core_send_cmd(struct wifi_core *wifi, const char *cmd, char *resp,
    size_t resp_size, uint32_t timeout_ms) {
	at_result_t result;

	if (!wifi || !cmd) {
		return -EINVAL;
	}

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

	/* Debug output */
#ifdef CONFIG_WIFI_CORE_DEBUG
	if (resp && resp_size > 0 && strlen(resp) > 0) {
		int resp_len = strlen(resp);
		char debug_buf[101];
		strncpy(debug_buf, resp, 100);
		debug_buf[100] = '\0';

		wifi_core_debug(wifi, "received %d bytes response", resp_len);
		wifi_core_debug(wifi, "response: %s%s", debug_buf,
		    resp_len > 100 ? "..." : "");
	}
#endif

	wifi->stats.tx_packets++;
	return 0;
}
