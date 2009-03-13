/*
 * net_device.c
 *
 *  Created on: Mar 4, 2009
 *      Author: anton
 */
#include "types.h"
#include "net_device.h"

#define MAX_ETHDEV 8
static net_device net_devices[MAX_ETHDEV];
static char status_net_devs[MAX_ETHDEV];
static inline int dev_is_busy(int num)
{
	return status_net_devs[num];
}
static inline net_device *dev_lock(int num)
{
	status_net_devs[num] = 1;
	return &net_devices[num];
}
net_device *alloc_etherdev(int inum)
{
	int i;
	if (0 == inum)
	{
		for (i = 0; i < MAX_ETHDEV; i ++){
			if (!dev_is_busy(i))
			{
				return dev_lock(i);
			}
		}
		return NULL;
	}
	if (!dev_is_busy(inum))
	{
		return dev_lock(inum);
	}
	return NULL;
}

net_device *find_net_device(const char * name)
{
	return NULL;
}
