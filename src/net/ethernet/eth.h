/**
 * \file eth.h
 *
 * \date Mar 5, 2009
 * \author anton
 */

#ifndef ETH_H_
#define ETH_H_

#define NET_TYPE_ALL_PROTOCOL 0

#include "core/net_device.h"

typedef void (*ETH_LISTEN_CALLBACK)(void * pack);

/**
 * Init ethernet.
 * @return count founded devices.
 */
int eth_init();

/**
 * Show interace (IP/MAC address)
 */


/**
 * Show all eth interfaces (IP/MAC address)
 */

/**
 * Send Ethernet packet
 */
int eth_send (net_packet *pack);

#endif /* ETH_H_ */
