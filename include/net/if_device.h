/**
 * \file if_device.h
 * \date Jul 18, 2009
 * \author Anton Bondarev
 */
#ifndef IF_DEVICE_H_
#define IF_DEVICE_H_

#include "net/eth.h"

/**
 * struct of interface device
 */
typedef struct _IF_DEVICE {
    unsigned char ipv4_addr[IPV4_ADDR_LENGTH];
    net_device_type    *net_dev;
    unsigned char mask[IPV4_ADDR_LENGTH];
    unsigned char gw[IPV4_ADDR_LENGTH];
    unsigned char ipv4_addr_length;
} IF_DEVICE;

int ifdev_init();

/**
 * get pointer on net_device struct linked with pointed interface
 * @param interface handler
 * @return pointer on net_device struct, NULL if error
 */
net_device_type *ifdev_get_netdevice(void *handler);

/**
 * set callback function for all incoming packet throw interface
 * use it in sniffer program
 * not need set interface to PROMISC mode now
 */
int ifdev_listen(void *handler, unsigned short type, ETH_LISTEN_CALLBACK callback);

/**
 * find known ifdev device has pointed ip address
 * @param ipaddr - ip devices address
 */
int ifdev_find_by_ip(const unsigned char *ipaddr);

/**
 * Get ifdev by name
 * @param if_name - interface name
 */
void *ifdev_find_by_name(const char *if_name);

/**
 * set all config value for pointed interface
 * @param name - interface name
 * @param ipaddr - ip devices address
 * @param macaddr - MAC devices address
 */
int ifdev_set_interface(char *name, char *ipaddr, char *macaddr);

/**
 * Set IP address (sw)
 * @param ipaddr - ip devices address
 */
int ifdev_set_ipaddr(void *ifdev, const unsigned char *ipaddr);

/**
 * Set MAC address
 * @param macaddr - MAC devices address
 */
int ifdev_set_macaddr(void *ifdev, const unsigned char *macaddr);

/**
 * Get interface's IP address
 * @param handler interface handler
 */
unsigned char *ifdev_get_ipaddr(void *handler);

/* iterator functions */
IF_DEVICE * ifdev_get_fist_used();
IF_DEVICE * ifdev_get_next_used();


/**
 * Show interace (IP/MAC address)
 * @param ifdev interface handler
 */

/**
 * Show all eth interfaces (IP/MAC address)
 */

#endif /* IF_DEVICE_H_ */
