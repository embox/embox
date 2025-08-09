/**
 * @file
 * @brief ESP32-C3 WiFi Driver Header
 *
 * @date Aug 6, 2025
 * @author Peize Li
 */

#ifndef DRIVERS_NET_ESP32C3_WIFI_H_
#define DRIVERS_NET_ESP32C3_WIFI_H_

#include <at/at_client.h>
#include <net/netdevice.h>
#include <wifi/wifi_core.h>

/**
 * @brief ESP32-C3 WiFi driver structure
 */
struct esp32c3_wifi {
	struct wifi_core core;  /* WiFi Core instance */
	at_client_t *at_client; /* AT client */

	bool owns_at_client_fd; /* Ownership flag for AT client file descriptor */

	/* ESP32 specific data */
	int wifi_mode; /* 1=STA, 2=AP, 3=STA+AP */
	bool initialized;
	char fw_version[64];

	/* Connection error code mapping */
	int last_error_code;

	int power_mode; /* Current power mode */
};

/**
 * @brief Initialize ESP32-C3 WiFi driver with file descriptor
 *
 * @param netdev Network device
 * @param fd Open file descriptor (e.g., PTY, socket)
 * @return 0 on success, negative on error
 */
extern int esp32c3_wifi_init_fd(struct net_device *netdev, int fd);

/**
 * @brief Initialize ESP32-C3 WiFi driver
 *
 * @param netdev Network device
 * @param uart_path UART device path (e.g. "/dev/ttyS1")
 * @return 0 on success, negative on error
 */
extern int esp32c3_wifi_init(struct net_device *netdev, const char *uart_path);

/**
 * @brief Cleanup ESP32-C3 WiFi driver
 *
 * @param netdev Network device
 */
extern void esp32c3_wifi_cleanup(struct net_device *netdev);

/**
 * @brief Set power mode
 *
 * @param wifi WiFi core instance
 * @param mode Power mode (WLAN_PWR_ACTIVE, WLAN_PWR_SAVE, WLAN_PWR_DEEP_SAVE)
 * @return 0 on success, negative on error
 */
extern int esp32c3_set_power_mode(struct wifi_core *wifi, int mode);

/**
 * @brief Get current power mode
 *
 * @param wifi WiFi core instance
 * @param mode Pointer to store current power mode
 * @return 0 on success, negative on error
 */
extern int esp32c3_get_power_mode(struct wifi_core *wifi, int *mode);

#endif /* DRIVERS_NET_ESP32C3_WIFI_H_ */