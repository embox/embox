/**
 * @file
 * @brief Wireless Device IOCTL Operations Implementation
 * 
 * @date Jul 27, 2025
 * @author Peize Li
 */
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>

#include <net/wlan/wireless_ioctl.h>
#include <net/wlan/wlan_core.h>

int wireless_sock_ioctl(int cmd, void *arg) {
	struct iwreq *iwr = (struct iwreq *)arg;
	struct net_device *dev;
	struct wlan_device *wdev;

	/* Find network device by interface name */
	dev = wlan_device_get_by_name(iwr->ifr_name);
	if (!dev) {
		return -ENODEV;
	}

	/* Get wlan_device from net_device */
	wdev = (struct wlan_device *)dev->wireless_priv;
	if (!wdev) {
		return -EINVAL;
	}

	switch (cmd) {
	case SIOCSIWESSID: {
		char ssid[33];
		if (iwr->u.essid.length > 32)
			return -EINVAL;

		memcpy(ssid, iwr->u.essid.pointer, iwr->u.essid.length);
		ssid[iwr->u.essid.length] = '\0';

		return wlan_connect(dev->name, ssid, NULL);
	}

	case SIOCGIWESSID: {
		mutex_lock(&wdev->lock);
		if (wdev->state == WLAN_STATE_CONNECTED) {
			size_t len = strlen(wdev->connected.ssid);
			if (len > iwr->u.essid.length)
				len = iwr->u.essid.length;

			memcpy(iwr->u.essid.pointer, wdev->connected.ssid, len);
			iwr->u.essid.length = len;
			iwr->u.essid.flags = 1;
		}
		else {
			iwr->u.essid.length = 0;
			iwr->u.essid.flags = 0;
		}
		mutex_unlock(&wdev->lock);
		return 0;
	}

	case SIOCSIWSCAN:
		return wlan_scan(dev->name);

	case SIOCGIWSCAN: {
		/* Return scan results */
		char *buf = iwr->u.data.pointer;
		int maxlen = iwr->u.data.length;
		int len = 0;

		mutex_lock(&wdev->lock);
		for (int i = 0; i < wdev->scan_result.count && len < maxlen - 100; i++) {
			struct wlan_scan_ap *ap = &wdev->scan_result.aps[i];
			int n = snprintf(buf + len, maxlen - len,
			    "ESSID:\"%s\" Signal:%d Channel:%d%s\n", ap->ssid, ap->rssi,
			    ap->channel, ap->security ? " Encryption:on" : "");
			if (n > 0)
				len += n;
		}
		mutex_unlock(&wdev->lock);

		iwr->u.data.length = len;
		return 0;
	}

	case SIOCSIWMODE: {
		if (!dev->wireless_ops->set_mode) {
			return -EOPNOTSUPP;
		}
		return dev->wireless_ops->set_mode(dev, iwr->u.mode);
	}

	case SIOCGIWMODE: {
		if (!dev->wireless_ops->get_mode) {
			return -EOPNOTSUPP;
		}
		return dev->wireless_ops->get_mode(dev, &iwr->u.mode);
	}

	default:
		return -EOPNOTSUPP;
	}
}