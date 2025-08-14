/**
 * @file
 * @brief Wireless Device IOCTL Operations Header
 * 
 * @date Jul 27, 2025
 * @author Peize Li
 */

#ifndef NET_WIRELESS_IOCTL_H_
#define NET_WIRELESS_IOCTL_H_

#include <stdint.h>

#include <net/netdevice.h>

/* Wireless device operations - structure declared in netdevice.h */
struct wireless_ops {
	int (*scan)(struct net_device *dev);
	int (*connect)(struct net_device *dev, const char *ssid, const char *pwd);
	int (*disconnect)(struct net_device *dev);
	int (*get_rssi)(struct net_device *dev, int8_t *rssi);

	int (*init)(struct net_device *dev);
	void (*deinit)(struct net_device *dev);
	int (*set_power_mode)(struct net_device *dev, int mode);
	int (*get_power_mode)(struct net_device *dev, int *mode);
	int (*set_mode)(struct net_device *dev, int mode); /* STA/AP/Monitor */
	int (*get_mode)(struct net_device *dev, int *mode);
};

/* Power modes */
#define WLAN_PWR_ACTIVE    0
#define WLAN_PWR_SAVE      1
#define WLAN_PWR_DEEP_SAVE 2

/* Operation modes */
#define WLAN_MODE_STA     1
#define WLAN_MODE_AP      2
#define WLAN_MODE_MONITOR 3

/**
 * @brief Perform wireless socket IOCTL operation
 *
 * Legacy interface for performing various wireless control operations
 * through socket IOCTL interface.
 *
 * @param cmd IOCTL command code
 * @param arg Pointer to command-specific argument structure
 * @return 0 on success, negative error code on failure
 */
extern int wireless_sock_ioctl(int cmd, void *arg);

#endif /* NET_WIRELESS_IOCTL_H_ */