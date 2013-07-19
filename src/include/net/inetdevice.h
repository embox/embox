/**
 * @file
 * @brief
 *
 * @date 18.07.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef NET_INETDEVICE_H_
#define NET_INETDEVICE_H_

#include <net/netdevice.h>
#include <netinet/in.h>
#include <util/list.h>

struct net_device;

/**
 * structute of inet(IP) device
 */
typedef struct in_device {
	struct list_link lnk;
	struct net_device *dev;

	in_addr_t ifa_address;
	in_addr_t ifa_mask;
	in_addr_t ifa_broadcast;

	struct in6_addr ifa6_address;

	unsigned int ip_id_generator;
} in_device_t;

extern int inetdev_register_dev(struct net_device *dev);

extern int inetdev_unregister_dev(struct net_device *dev);

/**
 * Get inet_devive by name
 * @param if_name - interface name
 */
extern struct in_device * inetdev_get_by_name(const char *name);

/**
 * get pointer on in_device struct linked with pointed net_device
 * @param dev
 * @return pointer on in_device struct, NULL if error
 */
extern struct in_device * inetdev_get_by_dev(struct net_device *dev);

/**
 * find known netdev device has pointed ip address
 * @param ipaddr - ip devices address
 */
extern struct in_device * inetdev_get_by_addr(in_addr_t addr);

/**
 * Get loopback device. Probably temporarily needed
 * @return pointer to the loopback device
 */
extern struct in_device * inetdev_get_loopback_dev(void);

/**
 * Set IP address (sw)
 * @param ipaddr - ip devices address
 */
extern int inetdev_set_addr(struct in_device *in_dev, in_addr_t addr);

/**
 * Set IP mask
 * @param mask - ip mask
 */
extern int inetdev_set_mask(struct in_device *in_dev, in_addr_t mask);

/**
 * Set broadcast addres
 * @param bcast - broadcast protocol address
 */
extern int inetdev_set_bcast(struct in_device *in_dev, in_addr_t bcast);

/**
 * Get interface's IP address
 * @param in_dev interface handler
 */
extern in_addr_t inetdev_get_addr(struct in_device *in_dev);

/* iterator functions */
extern struct in_device * inetdev_get_first(void);
extern struct in_device * inetdev_get_next(struct in_device *in_dev);

/**
 * generate new id for requiered interface
 */
static inline unsigned int inetdev_get_ip_id(struct in_device *in_dev) {
	assert(in_dev != NULL);
	return ++in_dev->ip_id_generator;
}

#include <stdbool.h>
/**
 * perform check: does this ip should be processes by local ip stack
 * (This function is intended for external packets processing.
 * It DOESN'T support internal stack conventions.)
 * @param ipaddr - ip address
 * @param check_broadcast - should we check broadcast addresses
 * @param check_multicast - should we check multicast addresses
 */
extern bool ip_is_local(in_addr_t addr, bool check_broadcast, bool check_multicast);

#endif /* NET_INETDEVICE_H_ */
