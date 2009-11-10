/**
 * \file net_device.c
 *
 * \date Mar 4, 2009
 * \author anton
 */
#include "conio.h"
#include "string.h"
#include "asm/types.h"
#include "net/net.h"
#include "net/net_device.h"

typedef struct _NET_DEV_INFO {
	net_device_t       dev;
	net_device_stats stats;
	int              is_busy;
} NET_DEV_INFO;

static NET_DEV_INFO net_devices[NET_DEVICES_QUANTITY];

static inline int dev_is_busy(int num) {
	return net_devices[num].is_busy;
}

static inline net_device_t *dev_lock(int num) {
	net_devices[num].is_busy = 1;
	return &net_devices[num].dev;
}

static inline void dev_unlock(int num) {
	net_devices[num].is_busy = 0;
}

net_device_t *alloc_netdev() {
	int i;
	for (i = 0; i < NET_DEVICES_QUANTITY; i++) {
		if (!dev_is_busy(i)) {
			return dev_lock(i);
		}
	}
	return NULL;
}

void free_netdev(net_device_t *dev) {
        int i;
	for(i = 0; i < NET_DEVICES_QUANTITY; i++) {
		if (dev == &net_devices[i].dev) {
    			dev_unlock(i);
    		}
	}
}

net_device_t *netdev_get_by_name(const char *name) {
	int i;
	for (i = 0; i < NET_DEVICES_QUANTITY; i++) {
		if (dev_is_busy(i) &&
		   (0 == strncmp(name, net_devices[i].dev.name, sizeof (net_devices[i].dev.name)))) {
			return &net_devices[i].dev;
		}
	}
	return NULL;
}

net_device_stats *get_eth_stat(net_device_t *dev) {
        int i;
        for(i = 0; i < NET_DEVICES_QUANTITY; i++) {
                if (dev == &net_devices[i].dev) {
                        return &net_devices[i].stats;
                }
	}
}
