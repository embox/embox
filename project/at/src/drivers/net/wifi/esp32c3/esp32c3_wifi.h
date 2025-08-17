/**
 * @file
 * @brief ESP32-C3 WiFi Driver Header
 *
 * @date Aug 6, 2025
 * @author Peize Li
 */

#ifndef DRIVERS_NET_ESP32C3_WIFI_H_
#define DRIVERS_NET_ESP32C3_WIFI_H_

#include <stdbool.h>
#include <stdint.h>

#include <net/cfg80211.h>

/* ESP32-C3 WiFi modes */
#define ESP32_WIFI_MODE_NULL   0 /* Null mode */
#define ESP32_WIFI_MODE_STA    1 /* Station mode */
#define ESP32_WIFI_MODE_AP     2 /* Access Point mode */
#define ESP32_WIFI_MODE_STA_AP 3 /* Station + AP mode */

/* ESP32-C3 power saving modes */
#define ESP32_POWER_ACTIVE      0 /* No power saving */
#define ESP32_POWER_MODEM_SLEEP 1 /* Modem sleep */
#define ESP32_POWER_LIGHT_SLEEP 2 /* Light sleep */

/* ESP32-C3 connection error codes */
#define ESP32_ERR_TIMEOUT    1 /* Connection timeout */
#define ESP32_ERR_WRONG_PASS 2 /* Wrong password */
#define ESP32_ERR_NO_AP      3 /* Cannot find target AP */
#define ESP32_ERR_CONN_FAIL  4 /* Connection failed */
#define ESP32_ERR_DHCP_FAIL  5 /* DHCP failed */

/* ESP32-C3 disconnect reasons */
#define ESP32_REASON_UNSPECIFIED    1
#define ESP32_REASON_AUTH_EXPIRE    2
#define ESP32_REASON_AUTH_LEAVE     3
#define ESP32_REASON_ASSOC_EXPIRE   4
#define ESP32_REASON_ASSOC_TOOMANY  5
#define ESP32_REASON_NOT_AUTHED     6
#define ESP32_REASON_NOT_ASSOCED    7
#define ESP32_REASON_ASSOC_LEAVE    8
#define ESP32_REASON_ASSOC_NOT_AUTH 9
#define ESP32_REASON_BEACON_TIMEOUT 200
#define ESP32_REASON_NO_AP_FOUND    201
#define ESP32_REASON_AUTH_FAIL      202
#define ESP32_REASON_ASSOC_FAIL     203
#define ESP32_REASON_HANDSHAKE_FAIL 204

/* Configuration limits */
#define ESP32_MAX_SSID_LEN   32
#define ESP32_MAX_PASSWD_LEN 64
#define ESP32_MAX_SCAN_AP    20
#define ESP32_MAC_ADDR_LEN   6

/**
 * @brief ESP32-C3 status information
 * 
 * This structure can be retrieved for debugging purposes
 */
struct esp32c3_status {
	/* Connection status */
	bool connected;
	char ssid[ESP32_MAX_SSID_LEN + 1];
	uint8_t bssid[ESP32_MAC_ADDR_LEN];
	int8_t rssi;
	uint8_t channel;

	/* IP configuration */
	char ip_addr[16];
	char netmask[16];
	char gateway[16];
	char dns1[16];
	char dns2[16];

	/* Module information */
	char fw_version[64];
	uint8_t mac_addr[ESP32_MAC_ADDR_LEN];

	/* Statistics */
	uint32_t tx_packets;
	uint32_t rx_packets;
	uint32_t tx_errors;
	uint32_t rx_errors;
	uint32_t connect_time_ms;   /* Last connection time */
	int last_error_code;        /* Last error code */
	int last_disconnect_reason; /* Last disconnect reason */
};

/**
 * @brief ESP32-C3 configuration parameters
 */
struct esp32c3_config {
	/* UART configuration */
	const char *uart_path; /* UART device path */

	/* Timeouts (milliseconds) */
	uint32_t connect_timeout; /* Connection timeout (default 20000) */
	uint32_t scan_timeout;    /* Scan timeout (default 10000) */
	uint32_t cmd_timeout;     /* Command timeout (default 5000) */

	/* Power management */
	int power_mode;  /* Initial power mode */
	bool auto_sleep; /* Enable auto sleep */

	/* Debug options */
	bool at_debug; /* Enable AT debug output */
	bool verbose;  /* Enable verbose logging */
};

/* Main initialization functions */

/**
 * @brief Initialize ESP32-C3 WiFi with configuration
 * 
 * @param name Device name (e.g., "wlan0")
 * @param config Configuration parameters (NULL for defaults)
 * @return Allocated wiphy on success, NULL on failure
 */
extern struct wiphy *esp32c3_wifi_create(const char *name,
    const struct esp32c3_config *config);

/**
 * @brief Initialize ESP32-C3 WiFi with file descriptor
 * 
 * @param name Device name (e.g., "wlan0")
 * @param fd Open file descriptor (e.g., PTY, socket)
 * @return Allocated wiphy on success, NULL on failure
 */
extern struct wiphy *esp32c3_wifi_init_fd(const char *name, int fd);

/**
 * @brief Initialize ESP32-C3 WiFi with UART path
 * 
 * @param name Device name (e.g., "wlan0")
 * @param uart_path UART device path (e.g., "/dev/ttyS1")
 * @return Allocated wiphy on success, NULL on failure
 */
extern struct wiphy *esp32c3_wifi_init(const char *name, const char *uart_path);

/**
 * @brief Cleanup ESP32-C3 WiFi
 * 
 * @param wiphy WiFi device to cleanup
 */
extern void esp32c3_wifi_cleanup(struct wiphy *wiphy);

/* Status and control functions */

/**
 * @brief Get ESP32-C3 status information
 * 
 * @param wiphy WiFi device
 * @param status Status structure to fill
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_get_status(struct wiphy *wiphy, struct esp32c3_status *status);

/**
 * @brief Set WiFi operation mode
 * 
 * @param wiphy WiFi device
 * @param mode Operation mode (ESP32_WIFI_MODE_*)
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_set_wifi_mode(struct wiphy *wiphy, int mode);

/**
 * @brief Get current WiFi operation mode
 * 
 * @param wiphy WiFi device
 * @param mode Pointer to store current mode
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_get_wifi_mode(struct wiphy *wiphy, int *mode);

/**
 * @brief Set power saving mode
 * 
 * @param wiphy WiFi device
 * @param mode Power mode (ESP32_POWER_*)
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_set_power_mode(struct wiphy *wiphy, int mode);

/**
 * @brief Get current power saving mode
 * 
 * @param wiphy WiFi device
 * @param mode Pointer to store current power mode
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_get_power_mode(struct wiphy *wiphy, int *mode);

/**
 * @brief Reset ESP32-C3 module
 * 
 * @param wiphy WiFi device
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_reset(struct wiphy *wiphy);

/**
 * @brief Get firmware version
 * 
 * @param wiphy WiFi device
 * @param version Buffer to store version string (min 64 bytes)
 * @param size Buffer size
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_get_firmware_version(struct wiphy *wiphy, char *version,
    size_t size);

/**
 * @brief Set MAC address
 * 
 * @param wiphy WiFi device
 * @param mac MAC address (6 bytes)
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_set_mac_address(struct wiphy *wiphy, const uint8_t *mac);

/**
 * @brief Get MAC address
 * 
 * @param wiphy WiFi device
 * @param mac Buffer to store MAC address (6 bytes)
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_get_mac_address(struct wiphy *wiphy, uint8_t *mac);

/* Advanced features */

/**
 * @brief Enable/disable auto-connect
 * 
 * @param wiphy WiFi device
 * @param enable true to enable, false to disable
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_set_auto_connect(struct wiphy *wiphy, bool enable);

/**
 * @brief Set DHCP mode
 * 
 * @param wiphy WiFi device
 * @param enable true for DHCP, false for static IP
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_set_dhcp_mode(struct wiphy *wiphy, bool enable);

/**
 * @brief Set static IP configuration
 * 
 * @param wiphy WiFi device
 * @param ip IP address string
 * @param netmask Netmask string
 * @param gateway Gateway string
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_set_static_ip(struct wiphy *wiphy, const char *ip,
    const char *netmask, const char *gateway);

/**
 * @brief Send raw AT command (for debugging/testing)
 * 
 * @param wiphy WiFi device
 * @param cmd AT command string
 * @param response Response buffer (optional)
 * @param resp_size Response buffer size
 * @param timeout_ms Timeout in milliseconds
 * @return 0 on success, negative error code on failure
 */
extern int esp32c3_send_at_command(struct wiphy *wiphy, const char *cmd,
    char *response, size_t resp_size, uint32_t timeout_ms);

/* Helper macros for error checking */
#define ESP32C3_IS_CONNECTED(wiphy)                                        \
	({                                                                     \
		struct esp32c3_status __status;                                    \
		(esp32c3_get_status(wiphy, &__status) == 0 && __status.connected); \
	})

#define ESP32C3_GET_RSSI(wiphy)               \
	({                                        \
		struct esp32c3_status __status;       \
		esp32c3_get_status(wiphy, &__status); \
		__status.rssi;                        \
	})

#endif /* DRIVERS_NET_ESP32C3_WIFI_H_ */