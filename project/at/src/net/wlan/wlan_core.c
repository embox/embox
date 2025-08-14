/**
 * @file
 * @brief Wlan Core Framework Implementation
 *
 * @date July 27, 2025
 * @author Peize Li
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mem/sysmalloc.h>
#include <net/wlan/wireless_ioctl.h>
#include <net/wlan/wlan_core.h>

/* Default WLAN device */
static struct net_device *default_wlan_device = NULL;
static struct mutex wlan_global_lock = MUTEX_INIT_STATIC;

int wlan_device_init(struct net_device *dev) {
	struct wlan_device *wdev;
	int ret;

	if (!dev || !dev->wireless_ops) {
		return -EINVAL;
	}

	/* Allocate WLAN management structure */
	wdev = sysmalloc(sizeof(*wdev));
	if (!wdev) {
		return -ENOMEM;
	}

	memset(wdev, 0, sizeof(*wdev));
	mutex_init(&wdev->lock);
	wdev->state = WLAN_STATE_DOWN;
	wdev->driver_type = WLAN_DRV_GENERIC;

	/* Associate to net_device */
	dev->wireless_priv = wdev;
	dev->flags |= IFF_WIRELESS;

	/* Call driver's init function */
	if (dev->wireless_ops->init) {
		ret = dev->wireless_ops->init(dev);
		if (ret < 0) {
			printf("wlan: driver init failed for '%s': %d\n", dev->name, ret);
			sysfree(wdev);
			dev->wireless_priv = NULL;
			dev->flags &= ~IFF_WIRELESS;
			return ret;
		}
	}

	/* Device enters UP state after successful initialization */
	wdev->state = WLAN_STATE_UP;

	/* Set as default device */
	mutex_lock(&wlan_global_lock);
	if (!default_wlan_device) {
		default_wlan_device = dev;
	}
	mutex_unlock(&wlan_global_lock);

	printf("wlan: device '%s' initialized successfully\n", dev->name);
	return 0;
}

void wlan_device_cleanup(struct net_device *dev) {
	struct wlan_device *wdev;

	if (!dev || !(dev->flags & IFF_WIRELESS)) {
		return;
	}

	wdev = dev->wireless_priv;
	if (!wdev) {
		return;
	}

	/* Call driver's deinit function */
	if (dev->wireless_ops && dev->wireless_ops->deinit) {
		dev->wireless_ops->deinit(dev);
	}

	/* Clean up default device */
	mutex_lock(&wlan_global_lock);
	if (default_wlan_device == dev) {
		default_wlan_device = NULL;
	}
	mutex_unlock(&wlan_global_lock);

	sysfree(wdev);
	dev->wireless_priv = NULL;
	dev->flags &= ~IFF_WIRELESS;

	printf("wlan: device '%s' cleaned up\n", dev->name);
}

/* Register driver type and private data */
int wlan_device_register_driver(struct net_device *dev, int driver_type,
    void *driver_priv) {
	struct wlan_device *wdev;

	if (!dev || !(dev->flags & IFF_WIRELESS)) {
		return -EINVAL;
	}

	wdev = dev->wireless_priv;
	if (!wdev) {
		return -EINVAL;
	}

	mutex_lock(&wdev->lock);
	wdev->driver_type = driver_type;
	wdev->priv = driver_priv;
	mutex_unlock(&wdev->lock);

	return 0;
}

struct net_device *wlan_device_get_by_name(const char *name) {
	struct net_device *dev = netdev_get_by_name(name);

	if (dev && (dev->flags & IFF_WIRELESS)) {
		return dev;
	}

	return NULL;
}

struct net_device *wlan_device_get_default(void) {
	return default_wlan_device;
}

/* State management helpers */
void wlan_device_set_state(struct net_device *dev, enum wlan_state state) {
	struct wlan_device *wdev;

	if (!dev || !(dev->flags & IFF_WIRELESS)) {
		return;
	}

	wdev = dev->wireless_priv;
	if (!wdev) {
		return;
	}

	mutex_lock(&wdev->lock);
	wdev->state = state;
	mutex_unlock(&wdev->lock);

	printf("wlan: %s state changed to %d\n", dev->name, state);
}

enum wlan_state wlan_device_get_state(struct net_device *dev) {
	struct wlan_device *wdev;
	enum wlan_state state;

	if (!dev || !(dev->flags & IFF_WIRELESS)) {
		return WLAN_STATE_DOWN;
	}

	wdev = dev->wireless_priv;
	if (!wdev) {
		return WLAN_STATE_DOWN;
	}

	mutex_lock(&wdev->lock);
	state = wdev->state;
	mutex_unlock(&wdev->lock);

	return state;
}

/* Convenience function implementation */
int wlan_scan(const char *dev_name) {
	struct net_device *dev;
	struct wlan_device *wdev;

	if (dev_name) {
		dev = wlan_device_get_by_name(dev_name);
	}
	else {
		dev = wlan_device_get_default();
	}

	if (!dev || !dev->wireless_ops || !dev->wireless_ops->scan) {
		return -ENODEV;
	}

	wdev = dev->wireless_priv;
	if (!wdev) {
		return -EINVAL;
	}

	mutex_lock(&wdev->lock);

	if (wdev->state != WLAN_STATE_UP && wdev->state != WLAN_STATE_CONNECTED) {
		mutex_unlock(&wdev->lock);
		return -EINVAL;
	}

	/* For wifi_core drivers, state syncs automatically */
	if (wdev->driver_type != WLAN_DRV_WIFI_CORE) {
		wdev->state = WLAN_STATE_SCANNING;
	}
	mutex_unlock(&wdev->lock);

	int ret = dev->wireless_ops->scan(dev);

	/* For non-wifi_core drivers, state must be restored manually */
	if (wdev->driver_type != WLAN_DRV_WIFI_CORE) {
		mutex_lock(&wdev->lock);
		if (wdev->state == WLAN_STATE_SCANNING) {
			wdev->state = WLAN_STATE_UP;
		}
		mutex_unlock(&wdev->lock);
	}

	return ret;
}

int wlan_connect(const char *dev_name, const char *ssid, const char *pwd) {
	struct net_device *dev;
	struct wlan_device *wdev;

	if (!ssid)
		return -EINVAL;

	if (dev_name) {
		dev = wlan_device_get_by_name(dev_name);
	}
	else {
		dev = wlan_device_get_default();
	}

	if (!dev || !dev->wireless_ops || !dev->wireless_ops->connect) {
		return -ENODEV;
	}

	wdev = dev->wireless_priv;
	if (!wdev) {
		return -EINVAL;
	}

	mutex_lock(&wdev->lock);

	if (wdev->state != WLAN_STATE_UP) {
		mutex_unlock(&wdev->lock);
		return -EINVAL;
	}

	/* For wifi_core drivers, state syncs automatically */
	if (wdev->driver_type != WLAN_DRV_WIFI_CORE) {
		wdev->state = WLAN_STATE_CONNECTING;
	}
	mutex_unlock(&wdev->lock);

	int ret = dev->wireless_ops->connect(dev, ssid, pwd);

	/* For non-wifi_core drivers, state must be updated manually */
	if (wdev->driver_type != WLAN_DRV_WIFI_CORE) {
		mutex_lock(&wdev->lock);
		if (ret == 0) {
			wdev->state = WLAN_STATE_CONNECTED;
			strncpy(wdev->connected.ssid, ssid, 32);
			wdev->connected.ssid[32] = '\0';
		}
		else {
			wdev->state = WLAN_STATE_UP;
		}
		mutex_unlock(&wdev->lock);
	}

	return ret;
}

int wlan_disconnect(const char *dev_name) {
	struct net_device *dev;
	struct wlan_device *wdev;

	if (dev_name) {
		dev = wlan_device_get_by_name(dev_name);
	}
	else {
		dev = wlan_device_get_default();
	}

	if (!dev || !dev->wireless_ops || !dev->wireless_ops->disconnect) {
		return -ENODEV;
	}

	wdev = dev->wireless_priv;
	if (!wdev) {
		return -EINVAL;
	}

	int ret = dev->wireless_ops->disconnect(dev);

	/* For non-wifi_core drivers, state must be updated manually */
	if (wdev->driver_type != WLAN_DRV_WIFI_CORE) {
		mutex_lock(&wdev->lock);
		wdev->state = WLAN_STATE_UP;
		memset(&wdev->connected, 0, sizeof(wdev->connected));
		mutex_unlock(&wdev->lock);
	}

	return ret;
}

int wlan_get_scan_result(const char *dev_name, struct wlan_scan_ap *buf,
    int buf_count) {
	struct net_device *dev;
	struct wlan_device *wdev;
	int count;

	if (!buf || buf_count <= 0) {
		return -EINVAL;
	}

	if (dev_name) {
		dev = wlan_device_get_by_name(dev_name);
	}
	else {
		dev = wlan_device_get_default();
	}

	if (!dev) {
		return -ENODEV;
	}

	wdev = dev->wireless_priv;
	if (!wdev) {
		return -EINVAL;
	}

	mutex_lock(&wdev->lock);

	count = wdev->scan_result.count;
	if (count > buf_count) {
		count = buf_count;
	}

	if (count > 0 && wdev->scan_result.aps) {
		memcpy(buf, wdev->scan_result.aps, count * sizeof(struct wlan_scan_ap));
	}

	mutex_unlock(&wdev->lock);

	return count;
}

int wlan_set_power_mode(const char *dev_name, int mode) {
	struct net_device *dev;

	if (dev_name) {
		dev = wlan_device_get_by_name(dev_name);
	}
	else {
		dev = wlan_device_get_default();
	}

	if (!dev || !dev->wireless_ops || !dev->wireless_ops->set_power_mode) {
		return -ENODEV;
	}

	return dev->wireless_ops->set_power_mode(dev, mode);
}
