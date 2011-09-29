/**
 * @file
 * @brief implementation of the IP router.
 *
 * @date 16.11.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <net/route.h>
#include <net/arp.h>
#include <net/in.h>
#include <linux/init.h>
#include <errno.h>

/**
 * NOTE: Linux route uses 3 structs for routing:
 *    + Forwarding Information Base (FIB)
 *    - routing cache (256 chains)
 *    + neighbour table (ARP cache)
 */
static rt_entry_t rt_table[CONFIG_ROUTE_FIB_TABLE_SIZE];

int rt_add_route(net_device_t *dev, in_addr_t dst,
			in_addr_t mask, in_addr_t gw, int flags) {
	size_t i;

	for (i = 0; i < CONFIG_ROUTE_FIB_TABLE_SIZE; i++) {
		if (!(rt_table[i].rt_flags & RTF_UP)) {
			rt_table[i].dev        = dev;
			rt_table[i].rt_dst     = dst;
			rt_table[i].rt_mask    = mask;
			rt_table[i].rt_gateway = gw;
			rt_table[i].rt_flags   = RTF_UP | flags;
			return ENOERR;
		}
	}
	return -ENOMEM;
}

int rt_del_route(net_device_t *dev, in_addr_t dst,
			    in_addr_t mask, in_addr_t gw) {
	size_t i;

	for (i = 0; i < CONFIG_ROUTE_FIB_TABLE_SIZE; i++) {
		if ((rt_table[i].rt_dst == dst || INADDR_ANY == dst) &&
		    (rt_table[i].rt_mask == mask || INADDR_ANY == mask) &&
		    (rt_table[i].rt_gateway == gw || INADDR_ANY == gw)) {
			rt_table[i].rt_flags &= ~RTF_UP;
			return ENOERR;
		}
	}
	return -ENOENT;
}

int ip_route(sk_buff_t *skb) {
	size_t i;
	in_addr_t daddr;

	daddr = skb->nh.iph->daddr;
	for (i = 0; i < CONFIG_ROUTE_FIB_TABLE_SIZE; i++) {
		if (!(rt_table[i].rt_flags & RTF_UP)) {
			continue;
		}
		if ((daddr & rt_table[i].rt_mask) == rt_table[i].rt_dst) {
			skb->dev = rt_table[i].dev;
			// TODO even if type is SOCK_RAW?
			skb->nh.iph->saddr = in_dev_get(skb->dev)->ifa_address;
			if (rt_table[i].rt_gateway != INADDR_ANY) {
				skb->nh.iph->daddr = rt_table[i].rt_gateway;
				return arp_resolve(skb);
			}
			return ENOERR;
		}
	}
	return -ENOENT;
}

rt_entry_t * rt_fib_get_first(size_t *iter) {
	size_t i;

	for (i = 0; i < CONFIG_ROUTE_FIB_TABLE_SIZE; ++i) {
		if (rt_table[i].rt_flags & RTF_UP) {
			*iter = i + 1;
			return &rt_table[i];
		}
	}

	return NULL;
}

rt_entry_t * rt_fib_get_next(size_t *iter) {
	size_t i;

	for (i = *iter; i < CONFIG_ROUTE_FIB_TABLE_SIZE; ++i) {
		if (rt_table[i].rt_flags & RTF_UP) {
			*iter = i + 1;
			return &rt_table[i];
		}
	}

	return NULL;
}
