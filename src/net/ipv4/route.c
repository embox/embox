/**
 * @file
 * @brief implementation of the IP router.
 *
 * @date 16.11.09
 * @author Nikolay Korotky
 */

#include <net/route.h>
#include <net/skbuff.h>
#include <net/arp.h>
#include <net/in.h>
#include <linux/init.h>

#define RT_TABLE_SIZE 16

/**
 * NOTE: Linux route uses 3 structs for routing:
 *    + Forwarding Information Base (FIB)
 *    - routing cache (256 chains)
 *    - neibour table
 */
static rt_entry_t rt_table[RT_TABLE_SIZE];

int __init ip_rt_init(void) {
	devinet_init();
	return 0;
}

int rt_add_route(net_device_t *dev, in_addr_t dst,
			in_addr_t mask, in_addr_t gw, int flags) {
	size_t i;
	for (i = 0; i < RT_TABLE_SIZE; i++) {
		if (!(rt_table[i].rt_flags & RTF_UP)) {
			rt_table[i].dev        = dev;
			rt_table[i].rt_dst     = dst;
			rt_table[i].rt_mask    = mask;
			rt_table[i].rt_gateway = gw;
			rt_table[i].rt_flags   = RTF_UP|flags;
			return 0;
		}
	}
	return -1;
}

int rt_del_route(net_device_t *dev, in_addr_t dst,
			    in_addr_t mask, in_addr_t gw) {
	size_t i;
	for (i = 0; i < RT_TABLE_SIZE; i++) {
		if ((rt_table[i].rt_dst == dst || INADDR_ANY == dst) &&
		    (rt_table[i].rt_mask == mask || INADDR_ANY == mask) &&
		    (rt_table[i].rt_gateway == gw || INADDR_ANY == gw)) {
			rt_table[i].rt_flags &= ~RTF_UP;
			return 0;
		}
	}
	return -1;
}

int ip_route(sk_buff_t *skb) {
	size_t i;
	for (i = 0; i < RT_TABLE_SIZE; i++) {
		if (!(rt_table[i].rt_flags & RTF_UP)) {
			continue;
		}
		if ((skb->nh.iph->daddr & rt_table[i].rt_mask) == rt_table[i].rt_dst) {
			skb->dev = rt_table[i].dev;
			skb->nh.iph->saddr = in_dev_get(skb->dev)->ifa_address;
			if (rt_table[i].rt_gateway != INADDR_ANY) {
				skb->nh.iph->daddr = rt_table[i].rt_gateway;
				arp_find(skb->mac.ethh->h_dest, skb);
			}
			return 0;
		}
	}
	return -1;
}

static int rt_iter;

rt_entry_t *rt_fib_get_first() {
	for (rt_iter = 0; rt_iter < RT_TABLE_SIZE; rt_iter++) {
		if (rt_table[rt_iter].rt_flags & RTF_UP) {
			rt_iter++;
			return &rt_table[rt_iter - 1];
		}
	}
	return NULL;
}

rt_entry_t *rt_fib_get_next() {
	for (; rt_iter < RT_TABLE_SIZE; rt_iter++) {
		if (rt_table[rt_iter].rt_flags & RTF_UP) {
			rt_iter++;
			return &rt_table[rt_iter - 1];
		}
	}
	return NULL;
}
