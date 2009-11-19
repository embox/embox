/**
 * \file route.h
 * \date 16.11.09
 * \author sikmir
 * \brief Definitions for the IP router.
 * NOTE: no cache routing.
 */

#ifndef ROUTE_H_
#define ROUTE_H_

#include "types.h"
#include "net/skbuff.h"
#include "net/ip.h"
#include "lib/inet/netinet/in.h"

/**
 * Routing table entry.
 */
struct rt_entry {
	struct net_device *dev;
	in_addr_t rt_dst;
	__u32     rt_flags;
        in_addr_t rt_mask;
        in_addr_t rt_gateway;
        __u8      _is_up;
};

/**
 * Add new route to table.
 * @param dev Iface
 * @param dst Destination network
 * @param mask Genmask
 * @param gw Gateway
 */
int rt_add_route(struct net_device *dev, in_addr_t dst,
				in_addr_t mask, in_addr_t gw);

/**
 * Remove route from table.
 * @param dev Iface
 * @param dst Destination network
 * @param mask Genmask
 * @param gw Gateway
 */
int rt_del_route(struct net_device *dev, in_addr_t dst,
				in_addr_t mask, in_addr_t gw);

/**
 * Rebuild sk_buff according to appropriated route.
 * @param skbuff
 */
int ip_route(sk_buff_t *skbuff);

/**< iterators */
struct rt_entry *rt_fib_get_first();
struct rt_entry *rt_fib_get_next();

#endif /* ROUTE_H_ */
