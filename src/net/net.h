/*
 * net.h
 *
 *  Created on: Mar 11, 2009
 *      Author: anton
 */

#ifndef NET_H_
#define NET_H_

#include "net_device.h"
#include "eth.h"

int netif_rx(net_packet *pack);

#endif /* NET_H_ */
