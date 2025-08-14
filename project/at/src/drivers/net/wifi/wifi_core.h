/**
 * @file
 * @brief WiFi Core Framework Header
 * 
 * @date July 31, 2025
 * @author Peize Li
 */

#ifndef DRIVERS_NET_WIFI_CORE_H_
#define DRIVERS_NET_WIFI_CORE_H_

#include <stdbool.h>
#include <stdint.h>

#include <at/at_client.h>
#include <kernel/printk.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/time/timer.h>
#include <net/netdevice.h>
#include <net/wlan/wlan_core.h>

/* WiFi Core states */
enum wifi_core_state {
	WIFI_CORE_STATE_UNINIT = 0,
	WIFI_CORE_STATE_IDLE,
	WIFI_CORE_STATE_SCANNING,
	WIFI_CORE_STATE_CONNECTING,
	WIFI_CORE_STATE_WAIT_CONNECTED, /* Waiting for CONNECTED URC */
	WIFI_CORE_STATE_WAIT_IP,        /* Waiting for GOT IP URC */
	WIFI_CORE_STATE_CONNECTED,
	WIFI_CORE_STATE_ERROR
};

/* Forward declarations */
struct wifi_core;

/**
 * @brief WiFi hardware operations interface
 */
struct wifi_hw_ops {
	/* Hardware init/cleanup */
	int (*hw_init)(struct wifi_core *wifi);
	void (*hw_cleanup)(struct wifi_core *wifi);

	/* Scan operations */
	int (*parse_scan_line)(const char *line, struct wlan_scan_ap *ap);
	const char *(*get_scan_cmd)(void);

	/* Connection operations */
	int (*build_connect_cmd)(char *buf, size_t size, const char *ssid,
	    const char *pwd);
	const char *(*get_disconnect_cmd)(void);

	/* IP configuration */
	int (*get_ip_config)(struct wifi_core *wifi, char *ip, char *mask, char *gw);

	/* URC handler - returns true if handled */
	bool (*parse_urc)(struct wifi_core *wifi, const char *line);

	/* Signal strength */
	int (*get_rssi)(struct wifi_core *wifi, int8_t *rssi);

	/* Timeout configuration */
	struct {
		uint32_t scan_ms;
		uint32_t connect_ms;
		uint32_t cmd_ms;
	} timeouts;

	struct {
		const char *connected;    /* e.g. "WIFI CONNECTED" */
		const char *got_ip;       /* e.g. "WIFI GOT IP" */
		const char *disconnected; /* e.g. "WIFI DISCONNECT" */
	} urc_patterns;

	struct {
		const char *scan;       /* e.g. "AT+CWLAP" */
		const char *disconnect; /* e.g. "AT+CWQAP" */
	} basic_cmds;

	struct {
		const char *get_mode;     /* e.g. "AT+CWMODE?" */
		const char *set_mode_fmt; /* e.g. "AT+CWMODE=%d" */
	} mode_cmds;

	int (*parse_mode_response)(const char *resp, int *mode);
	int (*set_power_mode)(struct wifi_core *wifi, int mode);
	int (*get_power_mode)(struct wifi_core *wifi, int *mode);
};

/**
 * @brief WiFi Core Driver structure
 */
struct wifi_core {
	/* Associated network device */
	struct net_device *netdev;

	/* AT communication client */
	at_client_t *at_client;

	/* Hardware operations */
	const struct wifi_hw_ops *hw_ops;

	/* State machine */
	enum wifi_core_state state;
	struct mutex state_lock;

	/* Current connection info */
	struct {
		char ssid[33];
		char ip_addr[16];
		char netmask[16];
		char gateway[16];
		int8_t rssi;
		bool connected;
		bool has_ip;
	} conn_info;
	struct mutex conn_lock;

	/* Connection request */
	struct {
		char ssid[33];
		char password[65];
		bool has_password;
	} connect_req;

	/* Scan results */
	struct {
		struct wlan_scan_ap *aps;
		int count;
		int capacity;
	} scan;
	struct mutex scan_lock;

	/* Timers */
	struct sys_timer connect_timer;

	/* Statistics */
	struct {
		uint32_t tx_packets;
		uint32_t rx_packets;
		uint32_t tx_errors;
		uint32_t rx_errors;
		uint32_t connect_attempts;
		uint32_t connect_failures;
	} stats;

	/* Configuration options */
	struct {
		bool auto_reconnect;
		int retry_count;
		uint32_t retry_delay_ms;
		bool debug_urc;
	} config;

	/* Hardware specific private data */
	void *hw_priv;

	volatile bool connect_done;
	volatile int connect_result;
	volatile bool disconnect_done;
	volatile bool timer_active;
};

/**
 * @brief Initialize WiFi core
 * 
 * @param wifi WiFi core instance
 * @param netdev Network device
 * @param at_client AT client instance
 * @param hw_ops Hardware operations
 * @param hw_priv Hardware private data
 * @return 0 on success, negative on error
 */
extern int wifi_core_setup(struct wifi_core *wifi, struct net_device *netdev,
    at_client_t *at_client, const struct wifi_hw_ops *hw_ops, void *hw_priv);

/**
 * @brief Cleanup WiFi core
 * 
 * @param wifi WiFi core instance
 */
extern void wifi_core_cleanup(struct wifi_core *wifi);

/**
 * @brief Check if WiFi is connected
 * 
 * @param wifi WiFi core instance
 * @return true if connected, false otherwise
 */
extern bool wifi_core_is_connected(struct wifi_core *wifi);

/**
 * @brief Check if WiFi has IP address
 * 
 * @param wifi WiFi core instance
 * @return true if has IP, false otherwise
 */
extern bool wifi_core_has_ip(struct wifi_core *wifi);

/**
 * @brief Get connection information
 * 
 * @param wifi WiFi core instance
 * @param ssid SSID buffer (optional)
 * @param ip IP address buffer (optional)
 * @param rssi RSSI value pointer (optional)
 * @return 0 on success, negative on error
 */
extern int wifi_core_get_connection_info(struct wifi_core *wifi, char *ssid,
    char *ip, int8_t *rssi);

/**
 * @brief Send AT command
 * 
 * @param wifi WiFi core instance
 * @param cmd AT command
 * @param resp Response buffer (optional)
 * @param resp_size Response buffer size
 * @param timeout_ms Timeout in milliseconds
 * @return 0 on success, negative on error
 */
extern int wifi_core_send_cmd(struct wifi_core *wifi, const char *cmd,
    char *resp, size_t resp_size, uint32_t timeout_ms);

/**
 * @brief Set auto reconnect mode
 * 
 * @param wifi WiFi core instance
 * @param enable true to enable, false to disable
 */
extern void wifi_core_set_auto_reconnect(struct wifi_core *wifi, bool enable);

/**
 * @brief Set retry parameters
 * 
 * @param wifi WiFi core instance
 * @param count Retry count
 * @param delay_ms Delay between retries in milliseconds
 */
extern void wifi_core_set_retry_params(struct wifi_core *wifi, int count,
    uint32_t delay_ms);

/**
 * @brief Enable or disable URC debug
 * 
 * @param wifi WiFi core instance
 * @param enable true to enable, false to disable
 */
extern void wifi_core_set_debug_urc(struct wifi_core *wifi, bool enable);

/* WiFi Core wireless operations */
extern const struct wireless_ops wifi_core_ops;

/* Debug macros */
#ifdef CONFIG_WIFI_CORE_DEBUG
#define wifi_core_debug(wifi, fmt, ...)                                  \
	do {                                                                 \
		printk("wifi_core[%s]: " fmt "\n",                               \
		    (wifi)->netdev ? (wifi)->netdev->name : "?", ##__VA_ARGS__); \
	} while (0)
#else
#define wifi_core_debug(wifi, fmt, ...)
#endif

#define wifi_core_error(wifi, fmt, ...)      \
	printk("wifi_core[%s] ERROR: " fmt "\n", \
	    (wifi)->netdev ? (wifi)->netdev->name : "?", ##__VA_ARGS__)

#endif /* DRIVERS_NET_WIFI_CORE_H_ */