/*
 * @file
 *
 * @date 03.04.14
 * @author Ilia Vaprol
 */

#ifndef NET_SOCK_PORT_H_
#define NET_SOCK_PORT_H_

#include <netinet/in.h>
#include <util/indexator.h>

#define SOCK_PORT_DEF(name, capacity) \
	INDEX_CLAMP_DEF(name, 0, capacity, IPPORT_RESERVED, \
			IPPORT_USERRESERVED - 1);

#define SOCK_PORT_NONE INDEX_NONE

#define sock_port_alloc(name) index_alloc(name, INDEX_NEXT)
#define sock_port_lock        index_try_lock
#define sock_port_unlock      index_unlock

#endif /* NET_SOCK_PORT_H_ */
