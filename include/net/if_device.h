/**
 * \file if_device.h
 * \date Jul 18, 2009
 * \author Anton Bondarev
 */
#ifndef IF_DEVICE_H_
#define IF_DEVICE_H_

#include "net/eth.h"
#include "net/net.h"

/**
 * structute of inet(IP) device
 */
typedef struct inet_device {
    unsigned char ipv4_addr[IPV4_ADDR_LENGTH];
    struct net_device    *net_dev;
    unsigned char mask[IPV4_ADDR_LENGTH];
    /*TODO gw must be in route table*/
    //unsigned char gw[IPV4_ADDR_LENGTH];
    unsigned char ipv4_addr_length;
} inet_device_t;

int inet_dev_init();

/**
 * get pointer on net_device struct linked with pointed interface
 * @param interface handler
 * @return pointer on net_device struct, NULL if error
 */
struct net_device *inet_dev_get_netdevice(void *handler);

/**
 * set callback function for all incoming packet throw interface
 * use it in sniffer program
 * not need set interface to PROMISC mode now
 */
int inet_dev_listen(void *handler, unsigned short type, ETH_LISTEN_CALLBACK callback);

/**
 * find known ifdev device has pointed ip address
 * @param ipaddr - ip devices address
 */
int inet_dev_find_by_ip(const unsigned char *ipaddr);

/**
 * Get ifdev by name
 * @param if_name - interface name
 */
void *inet_dev_find_by_name(const char *if_name);

/**
 * set all config value for pointed interface
 * @param name - interface name
 * @param ipaddr - ip devices address
 * @param macaddr - MAC devices address
 */
int inet_dev_set_interface(char *name, char *ipaddr, char* mask, char *macaddr);

/**
 * Set IP address (sw)
 * @param ipaddr - ip devices address
 */
int inet_dev_set_ipaddr(void *ifdev, const unsigned char *ipaddr);

/**
 * Set IP mask
 * @param mask - ip mask
 */
int inet_dev_set_mask(void *ifdev, const unsigned char *mask);

/**
 * Set MAC address
 * @param macaddr - MAC devices address
 */
int inet_dev_set_macaddr(void *ifdev, const unsigned char *macaddr);

/**
 * Get interface's IP address
 * @param handler interface handler
 */
unsigned char *inet_dev_get_ipaddr(void *handler);

/* iterator functions */
inet_device_t * inet_dev_get_fist_used();
inet_device_t * inet_dev_get_next_used();


/**
 * Show interace (IP/MAC address)
 * @param ifdev interface handler
 */

/**
 * Show all eth interfaces (IP/MAC address)
 */

#endif /* IF_DEVICE_H_ */
