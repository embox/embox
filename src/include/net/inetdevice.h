/**
 * @file
 *
 * @date 18.07.2009
 * @author Anton Bondarev
 */
#ifndef INETDEVICE_H_
#define INETDEVICE_H_

#include <net/etherdevice.h>
#include <net/netdevice.h>
#include <net/net.h>

/**
 * structute of inet(IP) device
 */
typedef struct in_device {
	struct net_device *dev;
	in_addr_t         ifa_address;
	in_addr_t         ifa_mask;
	in_addr_t         ifa_broadcast;
	in_addr_t         ifa_anycast;
	unsigned char     ipv4_addr_length;
} in_device_t;

extern void devinet_init(void);

/**
 * get pointer on in_device struct linked with pointed net_device
 * @param dev
 * @return pointer on in_device struct, NULL if error
 */
extern in_device_t *in_dev_get(const net_device_t *dev);

/**
 * set callback function for all incoming packet throw interface
 * use it in sniffer program
 * not need set interface to PROMISC mode now
 */
extern int inet_dev_listen(in_device_t *in_dev, unsigned short type,
					    ETH_LISTEN_CALLBACK callback);

/**
 * find known netdev device has pointed ip address
 * @param ipaddr - ip devices address
 */
extern struct net_device *ip_dev_find(in_addr_t addr);

/**
 * Get inet_devive by name
 * @param if_name - interface name
 */
extern in_device_t *inet_dev_find_by_name(const char *if_name);

/**
 * set all config value for pointed interface
 * @param name - interface name
 * @param ipaddr - ip devices address
 * @param macaddr - MAC devices address
 */
extern int inet_dev_set_interface(const char *name, const in_addr_t ipaddr,
			    const in_addr_t mask, const unsigned char *macaddr);

/**
 * Set IP address (sw)
 * @param ipaddr - ip devices address
 */
extern int inet_dev_set_ipaddr(in_device_t *in_dev, const in_addr_t ipaddr);

/**
 * Set IP mask
 * @param mask - ip mask
 */
extern int inet_dev_set_mask(in_device_t *in_dev, const in_addr_t mask);

/**
 * Set MAC address
 * @param macaddr - MAC devices address
 */
extern int inet_dev_set_macaddr(in_device_t *in_dev, const unsigned char *macaddr);

/**
 * Get interface's IP address
 * @param in_dev interface handler
 */
extern in_addr_t inet_dev_get_ipaddr(in_device_t *in_dev);

/* iterator functions */
extern in_device_t * inet_dev_get_fist_used(void);
extern in_device_t * inet_dev_get_next_used(void);

/*TODO: deprecated*/
extern int ifdev_up(const char *iname);
extern int ifdev_down(const char *iname);

#endif /* INETDEVICE_H_ */
