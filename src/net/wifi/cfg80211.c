/**
 * @file
 * @brief cfg80211 Wireless Configuration API Implementation
 *
 * @date 26.07.25
 * @author Anton Bondarev
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <framework/mod/options.h>
#include <hal/clock.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/time/time.h>
#include <lib/libds/dlist.h>
#include <mem/misc/pool.h>
#include <net/cfg80211.h>
#include <net/inetdevice.h>
#include <net/netdevice.h>
#include <net/netlink.h>

#define MODOPS_CFG80211_BSS_MAX OPTION_GET(NUMBER, cfg80211_bss_max)
#define MODOPS_CFG80211_DEV_MAX OPTION_GET(NUMBER, cfg80211_dev_max)

/* BSS aging time - 30 seconds */
#define BSS_EXPIRE_TIME_MS 30000

POOL_DEF(cfg80211_bss_pool, struct cfg80211_bss, MODOPS_CFG80211_BSS_MAX);
POOL_DEF(cfg80211_dev_pool, struct cfg80211_registered_device,
    MODOPS_CFG80211_DEV_MAX);

/* Global list of registered devices */
static DLIST_DEFINE(cfg80211_dev_list);
static struct mutex cfg80211_dev_list_mutex;
static int cfg80211_initialized = 0;

/* Initialize cfg80211 subsystem */
static void cfg80211_init(void) {
	if (!cfg80211_initialized) {
		mutex_init(&cfg80211_dev_list_mutex);
		cfg80211_initialized = 1;
	}
}

/* Allocate new wiphy with private data */
struct wiphy *wiphy_new_nm(const struct cfg80211_ops *ops, int sizeof_priv,
    const char *requested_name) {
	struct cfg80211_registered_device *rdev;

	if (!ops) {
		return NULL;
	}

	cfg80211_init();

	rdev = pool_alloc(&cfg80211_dev_pool);
	if (!rdev) {
		return NULL;
	}

	memset(rdev, 0, sizeof(*rdev));
	rdev->ops = ops;

	dlist_head_init(&rdev->list);

	mutex_lock(&cfg80211_dev_list_mutex);
	dlist_add_prev(&rdev->list, &cfg80211_dev_list);
	mutex_unlock(&cfg80211_dev_list_mutex);

	return &rdev->wiphy;
}

/* Free wiphy and remove from global list */
void wiphy_free(struct wiphy *wiphy) {
	struct cfg80211_registered_device *rdev;

	if (!wiphy) {
		return;
	}

	rdev = wiphy_to_rdev(wiphy);

	mutex_lock(&cfg80211_dev_list_mutex);
	dlist_del(&rdev->list);
	mutex_unlock(&cfg80211_dev_list_mutex);

	pool_free(&cfg80211_dev_pool, rdev);
}

/* Register wireless device with cfg80211 */
void cfg80211_register_wdev(struct cfg80211_registered_device *rdev,
    struct wireless_dev *wdev) {
	if (!wdev) {
		return;
	}

	/* Initialize mutex for this wdev */
	mutex_init(&wdev->wdev_mutex);

	/* Initialize connection state */
	wdev->conn_state = CFG80211_DISCONNECTED;
	wdev->ssid_len = 0;
	memset(wdev->current_ssid, 0, sizeof(wdev->current_ssid));
	memset(wdev->current_bssid, 0, ETH_ALEN);
	wdev->current_signal = 0;

	/* Initialize scan list */
	dlist_init(&wdev->scan_list);
	wdev->scan_count = 0;
	wdev->scanning = false;
	wdev->last_scan_complete = 0;

	/* Initialize statistics */
	wdev->tx_packets = 0;
	wdev->rx_packets = 0;
	wdev->tx_failed = 0;

	/* Clear connection time */
	wdev->connect_time = 0;
}

/* Register network device with cfg80211 */
int cfg80211_register_netdevice(struct net_device *dev) {
	struct wireless_dev *wdev;
	struct cfg80211_registered_device *rdev;

	if (!dev) {
		return -EINVAL;
	}

	wdev = dev->nd_ieee80211_ptr;
	if (!wdev || !wdev->wiphy) {
		return -EINVAL;
	}

	rdev = wiphy_to_rdev(wdev->wiphy);
	if (!rdev) {
		return -EINVAL;
	}
	/* FIXME register in inetdev_register now
    ret = netdev_register(dev);
    if (ret) {
        goto out;
    }
	*/

	/* Register wdev with cfg80211 */
	cfg80211_register_wdev(rdev, wdev);

	return 0;
}

/* Connect to wireless network */
int cfg80211_connect(struct net_device *dev, struct cfg80211_connect_params *sme) {
	struct wireless_dev *wdev;
	struct cfg80211_registered_device *rdev;
	const struct cfg80211_ops *ops;
	int ret;

	if (!dev || !sme) {
		return -EINVAL;
	}

	/* Validate SSID */
	if (!sme->ssid || sme->ssid_len == 0 || sme->ssid_len > IEEE80211_MAX_SSID_LEN) {
		return -EINVAL;
	}

	wdev = dev->nd_ieee80211_ptr;
	if (!wdev) {
		return -ENODEV;
	}

	rdev = wiphy_to_rdev(wdev->wiphy);
	if (!rdev) {
		return -EINVAL;
	}

	ops = rdev->ops;
	if (!ops || !ops->connect) {
		return -EOPNOTSUPP;
	}

	mutex_lock(&wdev->wdev_mutex);

	/* Check current state */
	if (wdev->conn_state == CFG80211_CONNECTED) {
		/* Already connected, disconnect first */
		mutex_unlock(&wdev->wdev_mutex);
		return -EALREADY;
	}

	if (wdev->conn_state == CFG80211_CONNECTING) {
		/* Connection in progress */
		mutex_unlock(&wdev->wdev_mutex);
		return -EINPROGRESS;
	}

	/* Update state to connecting */
	cfg80211_update_connection_state(wdev, CFG80211_CONNECTING);

	/* Save SSID */
	memcpy(wdev->current_ssid, sme->ssid, sme->ssid_len);
	wdev->ssid_len = sme->ssid_len;

	mutex_unlock(&wdev->wdev_mutex);

	/* Call driver connect operation */
	ret = ops->connect(wdev->wiphy, dev, sme);

	mutex_lock(&wdev->wdev_mutex);

	if (ret == 0) {
		/* Success - update state */
		cfg80211_update_connection_state(wdev, CFG80211_CONNECTED);
		wdev->connect_time = clock_sys_ticks();
	}
	else {
		/* Failed - clear state */
		cfg80211_update_connection_state(wdev, CFG80211_DISCONNECTED);
		wdev->ssid_len = 0;
		memset(wdev->current_ssid, 0, sizeof(wdev->current_ssid));
	}

	mutex_unlock(&wdev->wdev_mutex);

	return ret;
}

/* Disconnect from wireless network */
int cfg80211_disconnect(struct net_device *dev, uint16_t reason_code) {
	struct wireless_dev *wdev;
	struct cfg80211_registered_device *rdev;
	const struct cfg80211_ops *ops;
	int ret = 0;

	if (!dev) {
		return -EINVAL;
	}

	wdev = dev->nd_ieee80211_ptr;
	if (!wdev) {
		return -ENODEV;
	}

	mutex_lock(&wdev->wdev_mutex);

	/* Check if connected */
	if (wdev->conn_state != CFG80211_CONNECTED) {
		mutex_unlock(&wdev->wdev_mutex);
		return -ENOTCONN;
	}

	rdev = wiphy_to_rdev(wdev->wiphy);
	ops = rdev ? rdev->ops : NULL;

	mutex_unlock(&wdev->wdev_mutex);

	/* Call driver disconnect if available */
	if (ops && ops->disconnect) {
		ret = ops->disconnect(wdev->wiphy, dev, reason_code);
	}

	mutex_lock(&wdev->wdev_mutex);

	/* Update state regardless of driver return */
	cfg80211_update_connection_state(wdev, CFG80211_DISCONNECTED);
	wdev->ssid_len = 0;
	memset(wdev->current_ssid, 0, sizeof(wdev->current_ssid));
	memset(wdev->current_bssid, 0, ETH_ALEN);
	wdev->current_signal = 0;
	wdev->connect_time = 0;

	mutex_unlock(&wdev->wdev_mutex);

	return ret;
}

/* Update connection state - must be called with wdev_mutex held */
void cfg80211_update_connection_state(struct wireless_dev *wdev,
    enum cfg80211_conn_state new_state) {
	if (!wdev) {
		return;
	}

	/* Caller must hold wdev_mutex */
	wdev->conn_state = new_state;

	switch (new_state) {
	case CFG80211_DISCONNECTED:
		wdev->connect_time = 0;
		break;

	case CFG80211_CONNECTED:
		if (wdev->connect_time == 0) {
			wdev->connect_time = clock_sys_ticks();
		}
		break;

	case CFG80211_CONNECTING:
	case CFG80211_CONN_FAILED:
	default:
		break;
	}
}

/* Add or update BSS in scan list */
struct cfg80211_bss *cfg80211_add_bss(struct wireless_dev *wdev,
    const uint8_t *bssid, const uint8_t *ssid, uint8_t ssid_len, int16_t signal,
    uint32_t channel, uint8_t enc_type) {
	struct cfg80211_bss *bss, *found_bss = NULL;
	unsigned long current_time;

	if (!wdev || !bssid) {
		return NULL;
	}

	/* Validate SSID length */
	if (ssid_len > IEEE80211_MAX_SSID_LEN) {
		return NULL;
	}

	current_time = clock_sys_ticks();

	mutex_lock(&wdev->wdev_mutex);

	/* Search for existing BSS */
	dlist_foreach_entry(bss, &wdev->scan_list, list) {
		if (memcmp(bss->bssid, bssid, ETH_ALEN) == 0) {
			found_bss = bss;
			break;
		}
	}

	if (found_bss) {
		/* Update existing BSS */
		if (ssid && ssid_len > 0) {
			memcpy(found_bss->ssid, ssid, ssid_len);
			found_bss->ssid_len = ssid_len;
		}
		found_bss->signal = signal;
		found_bss->channel = channel;
		found_bss->last_seen = current_time;

		mutex_unlock(&wdev->wdev_mutex);
		return found_bss;
	}

	/* Check if we've reached max BSS count */
	if (wdev->scan_count >= MODOPS_CFG80211_BSS_MAX) {
		/* Remove oldest BSS */
		struct cfg80211_bss *oldest = NULL;
		unsigned long oldest_time = current_time;

		dlist_foreach_entry(bss, &wdev->scan_list, list) {
			if (bss->last_seen < oldest_time) {
				oldest = bss;
				oldest_time = bss->last_seen;
			}
		}

		if (oldest) {
			dlist_del(&oldest->list);
			pool_free(&cfg80211_bss_pool, oldest);
			wdev->scan_count--;
		}
	}

	/* Allocate new BSS */
	bss = pool_alloc(&cfg80211_bss_pool);
	if (!bss) {
		mutex_unlock(&wdev->wdev_mutex);
		return NULL;
	}

	/* Initialize BSS */
	memset(bss, 0, sizeof(*bss));
	memcpy(bss->bssid, bssid, ETH_ALEN);

	if (ssid && ssid_len > 0) {
		memcpy(bss->ssid, ssid, ssid_len);
		bss->ssid_len = ssid_len;
	}

	bss->signal = signal;
	bss->channel = channel;
	bss->last_seen = current_time;

	/* Privacy and encryption type */
	bss->privacy = (enc_type != 0) ? 1 : 0;
	switch (enc_type) {
	case 0: // Open
		bss->wpa_version = 0;
		break;
	case 1: // WEP
		bss->wpa_version = 0;
		break;
	case 2: // WPA
		bss->wpa_version = 1;
		break;
	case 3: // WPA2
		bss->wpa_version = 2;
		break;
	default:
		bss->wpa_version = 0;
		break;
	}

	/* Add to scan list */
	dlist_add_prev(&bss->list, &wdev->scan_list);
	wdev->scan_count++;

	mutex_unlock(&wdev->wdev_mutex);

	return bss;
}

int cfg80211_scan(struct net_device *dev, struct cfg80211_scan_request *request) {
	struct wireless_dev *wdev;
	struct cfg80211_registered_device *rdev;
	const struct cfg80211_ops *cfg80211_ops;
	int ret;

	if (NULL == dev) {
		return -EINVAL;
	}

	wdev = dev->nd_ieee80211_ptr;
	rdev = wiphy_to_rdev(wdev->wiphy);
	if (NULL == rdev) {
		return -EINVAL;
	}

	cfg80211_ops = rdev->ops;
	if (NULL == cfg80211_ops) {
		return -EINVAL;
	}

	ret = cfg80211_ops->scan(wdev->wiphy, request);

	return ret;
}

/* Clear all scan results */
void cfg80211_clear_scan_results(struct wireless_dev *wdev) {
	struct cfg80211_bss *bss;
	struct dlist_head *pos;

	if (!wdev) {
		return;
	}

	mutex_lock(&wdev->wdev_mutex);

	/* Remove all BSS entries */
	dlist_foreach_safe(pos, &wdev->scan_list) {
		bss = dlist_entry(pos, struct cfg80211_bss, list);
		dlist_del(&bss->list);
		pool_free(&cfg80211_bss_pool, bss);
	}

	wdev->scan_count = 0;

	mutex_unlock(&wdev->wdev_mutex);
}

/* Mark scan as complete */
void cfg80211_scan_done(struct wireless_dev *wdev, bool aborted) {
	struct cfg80211_bss *bss;
	struct dlist_head *pos;
	unsigned long current_time, expire_jiffies;

	if (!wdev) {
		return;
	}

	mutex_lock(&wdev->wdev_mutex);

	wdev->scanning = false;

	if (!aborted) {
		wdev->last_scan_complete = clock_sys_ticks();

		/* Age out old BSS entries */
		current_time = clock_sys_ticks();
		expire_jiffies = ms2jiffies(BSS_EXPIRE_TIME_MS);

		dlist_foreach_safe(pos, &wdev->scan_list) {
			bss = dlist_entry(pos, struct cfg80211_bss, list);
			/* Remove entries older than 30 seconds */
			if ((current_time - bss->last_seen) > expire_jiffies) {
				dlist_del(&bss->list);
				pool_free(&cfg80211_bss_pool, bss);
				wdev->scan_count--;
			}
		}
	}

	mutex_unlock(&wdev->wdev_mutex);
}
/* Get BSS by BSSID */
struct cfg80211_bss *cfg80211_get_bss(struct wireless_dev *wdev,
    const uint8_t *bssid) {
	struct cfg80211_bss *bss;

	if (!wdev || !bssid) {
		return NULL;
	}

	mutex_lock(&wdev->wdev_mutex);

	dlist_foreach_entry(bss, &wdev->scan_list, list) {
		if (memcmp(bss->bssid, bssid, ETH_ALEN) == 0) {
			mutex_unlock(&wdev->wdev_mutex);
			return bss;
		}
	}

	mutex_unlock(&wdev->wdev_mutex);

	return NULL;
}

/* Notify cfg80211 of connection result */
void cfg80211_connect_result(struct net_device *dev, const uint8_t *bssid,
    const uint8_t *req_ie, size_t req_ie_len, const uint8_t *resp_ie,
    size_t resp_ie_len, uint16_t status, unsigned int gfp) {
	struct wireless_dev *wdev;

	if (!dev)
		return;
	wdev = dev->nd_ieee80211_ptr;
	if (!wdev)
		return;

	/* update connection state */
	mutex_lock(&wdev->wdev_mutex);
	if (status == WLAN_STATUS_SUCCESS) {
		cfg80211_update_connection_state(wdev, CFG80211_CONNECTED);
	}
	else {
		cfg80211_update_connection_state(wdev, CFG80211_DISCONNECTED);
	}
	mutex_unlock(&wdev->wdev_mutex);
}

/* disconnect notification */
void cfg80211_disconnected(struct net_device *dev, uint16_t reason,
    const uint8_t *ie, size_t ie_len, bool locally_generated, unsigned int gfp) {
	struct wireless_dev *wdev;

	if (!dev)
		return;
	wdev = dev->nd_ieee80211_ptr;
	if (!wdev)
		return;

	mutex_lock(&wdev->wdev_mutex);
	cfg80211_update_connection_state(wdev, CFG80211_DISCONNECTED);
	mutex_unlock(&wdev->wdev_mutex);
}

/* unregister network device */
void cfg80211_unregister_netdevice(struct net_device *dev) {
	/* TODO: implement device unregistration logic */
	(void)dev;
}
