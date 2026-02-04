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
#include <kernel/time/time.h>
#include <kernel/time/sys_timer.h>
#include <net/cfg80211.h>
#include <net/netdevice.h>

/* Forward declarations */
struct wifi_core;

/**
 * @brief WiFi Access Point structure
 */
struct wlan_scan_ap {
	char ssid[33];    /* SSID string */
	uint8_t bssid[6]; /* BSSID MAC address */
	int8_t rssi;      /* Signal strength in dBm */
	uint8_t channel;  /* Channel number */
	uint8_t enc_type; /* Encryption type: 0=Open, 1=WEP, 2=WPA, 3=WPA2 */
};

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
 * 
 * This structure is now private to the driver and embedded in wiphy private data.
 * External code should only interact through wiphy and cfg80211 interfaces.
 */
struct wifi_core {
	/* cfg80211 infrastructure */
	struct wiphy *wiphy;
	struct wireless_dev wdev; /* Embedded wireless device */

	/* Associated network device */
	struct net_device *netdev;

	/* AT communication client */
	at_client_t *at_client;

	/* Hardware operations */
	const struct wifi_hw_ops *hw_ops;

	/* Synchronization */
	struct mutex state_lock; /* Protects state transitions */
	struct mutex conn_lock;  /* Protects connection info */
	struct mutex scan_lock;  /* Protects scan cache */

	/* Current connection info */
	struct {
		char ssid[33];
		char ip_addr[16];
		char netmask[16];
		char gateway[16];
		int8_t rssi;
	} conn_info;

	/* Scan cache for AT response parsing */
	struct {
		struct wlan_scan_ap *aps;
		int count;
		int capacity;
	} scan;

	/* Connection timer */
	struct sys_timer connect_timer;
	volatile bool timer_active;

	/* Statistics */
	struct {
		uint32_t tx_packets;
		uint32_t rx_packets;
		uint32_t tx_errors;
		uint32_t rx_errors;
		uint32_t connect_attempts;
		uint32_t connect_failures;
	} stats;

	struct {
		bool debug_urc;
		bool debug_scan;
		bool debug_cmd;
	} debug;

	/* Hardware specific private data */
	void *hw_priv;
};

/**
 * @brief Create WiFi device with cfg80211 support
 * 
 * Creates a new WiFi device with full cfg80211 integration. The device
 * will be automatically registered with both cfg80211 and network subsystems.
 * 
 * @param name Device name (e.g., "wlan0")
 * @param at_client AT client instance for hardware communication
 * @param hw_ops Hardware operations table
 * @param hw_priv Hardware private data (optional)
 * @return Allocated wiphy structure on success, NULL on failure
 */
extern struct wiphy *wifi_core_create(const char *name, at_client_t *at_client,
    const struct wifi_hw_ops *hw_ops, void *hw_priv);

/**
 * @brief Destroy WiFi device
 * 
 * Unregisters the device from cfg80211 and network subsystems,
 * cleanups all resources including timers and memory allocations.
 * 
 * @param wiphy Wiphy structure returned by wifi_core_create()
 */
extern void wifi_core_destroy(struct wiphy *wiphy);

/**
 * @brief Send AT command (internal helper)
 * 
 * This function is now primarily for internal use by hardware-specific
 * implementations. External code should use cfg80211 interfaces.
 * 
 * @param wifi WiFi core instance
 * @param cmd AT command string
 * @param resp Response buffer (optional)
 * @param resp_size Response buffer size
 * @param timeout_ms Timeout in milliseconds
 * @return 0 on success, negative error code on failure
 */
extern int wifi_core_send_cmd(struct wifi_core *wifi, const char *cmd,
    char *resp, size_t resp_size, uint32_t timeout_ms);

/**
 * @brief Get WiFi core from wiphy
 * 
 * Helper function to retrieve wifi_core structure from wiphy private data.
 * For use by hardware-specific implementations only.
 * 
 * @param wiphy Wiphy structure
 * @return Pointer to wifi_core structure
 */
static inline struct wifi_core *wiphy_to_wifi(struct wiphy *wiphy) {
	return wiphy_priv(wiphy);
}

// #define CONFIG_WIFI_CORE_DEBUG 1

/* Debug macros */
#ifdef CONFIG_WIFI_CORE_DEBUG
#define wifi_core_debug(wifi, fmt, ...)                                   \
	do {                                                                  \
		struct wifi_core *__wifi = (wifi);                                \
		if (__wifi && __wifi->netdev) {                                   \
			printk("[%lld ms] wifi_core[%s]: " fmt "\n",                  \
			    (long long)jiffies2ms(clock_sys_ticks()),                 \
			    __wifi->netdev->name, ##__VA_ARGS__);                     \
		}                                                                 \
		else {                                                            \
			printk("[%lld ms] wifi_core[?]: " fmt "\n",                   \
			    (long long)jiffies2ms(clock_sys_ticks()), ##__VA_ARGS__); \
		}                                                                 \
	} while (0)
#else
#define wifi_core_debug(wifi, fmt, ...)
#endif

#define wifi_core_error(wifi, fmt, ...)                                    \
	do {                                                                   \
		struct wifi_core *__wifi = (wifi);                                 \
		if (__wifi && __wifi->netdev) {                                    \
			printk("wifi_core[%s] ERROR: " fmt "\n", __wifi->netdev->name, \
			    ##__VA_ARGS__);                                            \
		}                                                                  \
		else {                                                             \
			printk("wifi_core[?] ERROR: " fmt "\n", ##__VA_ARGS__);        \
		}                                                                  \
	} while (0)

#endif /* DRIVERS_NET_WIFI_CORE_H_ */