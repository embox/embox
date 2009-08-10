/**
 * \file eth.h
 *
 * \date Mar 5, 2009
 * \author anton
 */

#ifndef ETH_H_
#define ETH_H_

#define NET_TYPE_ALL_PROTOCOL 0

#include "net/net_device.h"

typedef void (*ETH_LISTEN_CALLBACK)(void * pack);

/**
 * Functions provided by eth.c
 */

/**
 * Init ethernet.
 */
extern int eth_init();

/**
 * rebuild the Ethernet MAC header.
 * @param pack net packet to update
 */
extern int eth_rebuild_header(net_packet * pack);

/**
 * ether_setup - setup Ethernet network device
 * @param dev network device
 * Fill in the fields of the device structure with Ethernet-generic values.
 */
extern void ether_setup(net_device *dev);

/**
 * Allocates and sets up an Ethernet device
 */
extern net_device *alloc_etherdev(int num);

/**
 * Show interace (IP/MAC address)
 * @param ifdev interface handler
 */

/**
 * Show all eth interfaces (IP/MAC address)
 */

/**
 * Send Ethernet packet.
 * send packet into define eth interface
 * @param pack network packet which want send
 * @return on success, returns 0, on error, -1 is returned
 */
extern int eth_send (net_packet *pack);

#endif /* ETH_H_ */
