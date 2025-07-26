/**
 * @file
 * @brief
 *
 * @date 26.07.25
 * @author Anton Bondarev
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <lib/libds/dlist.h>

#include <mem/misc/pool.h>

#include <net/netlink.h>
#include <net/inetdevice.h>
#include <net/netdevice.h>

#include <framework/mod/options.h>

#include <net/cfg80211.h>

#define MODOPS_CFG80211_DEV_MAX OPTION_GET(NUMBER, cfg80211_dev_max)

POOL_DEF(cfg80211_dev_pool, struct cfg80211_registered_device, MODOPS_CFG80211_DEV_MAX);
static DLIST_DEFINE(cfg80211_dev_list);

struct wiphy *wiphy_new_nm(const struct cfg80211_ops *ops, int sizeof_priv,
                                    const char *requested_name) {
    struct cfg80211_registered_device *rdev;

    rdev = pool_alloc(&cfg80211_dev_pool);
    if (rdev == NULL) {
        return NULL;
    }

    rdev->ops = ops;

    return &rdev->wiphy; 
}

void cfg80211_register_wdev(struct cfg80211_registered_device *rdev,
                                    struct wireless_dev *wdev) {
}


int cfg80211_register_netdevice(struct net_device *dev) {
    struct wireless_dev *wdev = dev->nd_ieee80211_ptr;
    struct cfg80211_registered_device *rdev;
	int ret;

    rdev = wiphy_to_rdev(wdev->wiphy);
/* FIXME register in inetdev_register now
    ret = netdev_register(dev);
    if (ret) {
        goto out;
    }
*/
	cfg80211_register_wdev(rdev, wdev);
	ret = 0;

//out:
	return ret;
}

int cfg80211_connect(struct net_device *dev, struct cfg80211_connect_params *sme) {
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

    ret = cfg80211_ops->connect(wdev->wiphy, dev, sme);

	return ret;
}