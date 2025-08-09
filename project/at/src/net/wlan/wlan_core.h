/**
 * @file
 * @brief Wlan Core Framework Header
 *
 * @date July 27, 2025
 * @author Peize Li
 */

#ifndef NET_WLAN_CORE_H_
#define NET_WLAN_CORE_H_

#include <stdint.h>

#include <kernel/thread/sync/mutex.h>
#include <lib/libds/dlist.h>
#include <net/netdevice.h>

/* WLAN device states */
enum wlan_state {
	WLAN_STATE_DOWN = 0,
	WLAN_STATE_UP,
	WLAN_STATE_SCANNING,
	WLAN_STATE_CONNECTING,
	WLAN_STATE_CONNECTED
};

/* Scan results */
struct wlan_scan_ap {
	char ssid[33];
	uint8_t bssid[6];
	int8_t rssi;
	uint8_t channel;
	uint8_t security; /* 0:open, 1:wep, 2:wpa, 3:wpa2 */
};

/* WLAN device management structure - used as net_device->wireless_priv */
struct wlan_device {
	/* State management */
	enum wlan_state state;
	struct mutex lock;

	/* Current connection */
	struct {
		char ssid[33];
		uint8_t bssid[6];
		int8_t rssi;
	} connected;

	/* Scan result cache */
	struct {
		struct wlan_scan_ap *aps;
		int count;
		int capacity;
	} scan_result;

	/* Driver private data */
	void *priv;

	/* Driver type identifier */
	enum {
		WLAN_DRV_GENERIC = 0,
		WLAN_DRV_WIFI_CORE, /* Driver using wifi_core framework */
		WLAN_DRV_CUSTOM     /* Fully custom driver */
	} driver_type;
};

/* Basic operations */

/**
 * @brief Initialize WLAN device
 *
 * @param dev Network device to initialize for WLAN operations
 * @return 0 on success, negative error code on failure
 */
extern int wlan_device_init(struct net_device *dev);

/**
 * @brief Clean up WLAN device resources
 *
 * @param dev Network device to clean up
 */
extern void wlan_device_cleanup(struct net_device *dev);

/**
 * @brief Get WLAN device by name
 *
 * @param name Name of the network device to find
 * @return Pointer to the network device on success, NULL if not found
 */
extern struct net_device *wlan_device_get_by_name(const char *name);

/**
 * @brief Get default WLAN device
 *
 * @return Pointer to the default network device, NULL if no WLAN device exists
 */
extern struct net_device *wlan_device_get_default(void);

/**
 * @brief Scan for available wireless networks
 *
 * @param dev_name Name of the network device to perform scan
 * @return 0 on success, negative error code on failure
 */
extern int wlan_scan(const char *dev_name);

/**
 * @brief Connect to a wireless network
 *
 * @param dev_name Name of the network device to use for connection
 * @param ssid Service Set Identifier of the target network
 * @param pwd Password for network authentication (NULL for open networks)
 * @return 0 on success, negative error code on failure
 */
extern int wlan_connect(const char *dev_name, const char *ssid, const char *pwd);

/**
 * @brief Disconnect from current wireless network
 *
 * @param dev_name Name of the network device to disconnect
 * @return 0 on success, negative error code on failure
 */
extern int wlan_disconnect(const char *dev_name);

/**
 * @brief Get wireless network scan results
 *
 * @param dev_name Name of the network device to query
 * @param buf Buffer to store scan results
 * @param buf_count Maximum number of results to retrieve
 * @return Number of scan results copied to buffer, negative error code on failure
 */
extern int wlan_get_scan_result(const char *dev_name, struct wlan_scan_ap *buf,
    int buf_count);

/**
 * @brief Set WLAN device power mode
 *
 * @param dev_name Name of the network device to configure
 * @param mode Power mode (WLAN_PWR_ACTIVE, WLAN_PWR_SAVE, or WLAN_PWR_DEEP_SAVE)
 * @return 0 on success, negative error code on failure
 */
extern int wlan_set_power_mode(const char *dev_name, int mode);

/* Driver registration helper */

/**
 * @brief Register WLAN driver with device
 *
 * @param dev Network device to register driver with
 * @param driver_type Type of driver (WLAN_DRV_GENERIC, WLAN_DRV_WIFI_CORE, or WLAN_DRV_CUSTOM)
 * @param driver_priv Driver-specific private data pointer
 * @return 0 on success, negative error code on failure
 */
extern int wlan_device_register_driver(struct net_device *dev, int driver_type,
    void *driver_priv);

/* State synchronization helpers */

/**
 * @brief Set WLAN device state
 *
 * @param dev Network device to update
 * @param state New state to set (WLAN_STATE_DOWN, UP, SCANNING, CONNECTING, or CONNECTED)
 */
extern void wlan_device_set_state(struct net_device *dev, enum wlan_state state);

/**
 * @brief Get current WLAN device state
 *
 * @param dev Network device to query
 * @return Current device state
 */
extern enum wlan_state wlan_device_get_state(struct net_device *dev);

#endif /* NET_WLAN_CORE_H_ */