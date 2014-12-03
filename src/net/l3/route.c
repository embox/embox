/**
 * @file
 * @brief implementation of the IP router.
 *
 * @date 16.11.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 * @author Vladimir Sokolov
 */

#include <errno.h>
#include <assert.h>
#include <net/l3/route.h>
#include <linux/in.h>
#include <mem/misc/pool.h>
#include <net/inetdevice.h>
#include <util/bit.h>
#include <util/dlist.h>
#include <util/member.h>
#include <net/skbuff.h>
#include <net/sock.h>

#include <framework/mod/options.h>

/**
 * NOTE: Linux route uses 3 structures for routing:
 *    + Forwarding Information Base (FIB)
 *    - routing cache (256 chains)
 *    + neighbour table (ARP cache)
 */

struct rt_entry_info {
	struct dlist_head lnk;
	struct rt_entry entry;
};

POOL_DEF(rt_entry_info_pool, struct rt_entry_info, OPTION_GET(NUMBER,route_table_size));
static DLIST_DEFINE(rt_entry_info_list);

int rt_add_route(struct net_device *dev, in_addr_t dst,
		in_addr_t mask, in_addr_t gw, int flags) {
	struct rt_entry_info *rt_info;

	if (dev == NULL) {
		return -EINVAL;
	}

	rt_info = (struct rt_entry_info *)pool_alloc(&rt_entry_info_pool);
	if (rt_info == NULL) {
		return -ENOMEM;
	}
	rt_info->entry.dev = dev;
	rt_info->entry.rt_dst = dst; /* We assume that host bits are zeroes here */
	rt_info->entry.rt_mask = mask;
	rt_info->entry.rt_gateway = gw;
	rt_info->entry.rt_flags = RTF_UP | flags;

	dlist_add_prev_entry(rt_info, &rt_entry_info_list, lnk);

	return 0;
}

int rt_del_route(struct net_device *dev, in_addr_t dst,
		in_addr_t mask, in_addr_t gw) {
	struct rt_entry_info *rt_info;

	dlist_foreach_entry(rt_info, &rt_entry_info_list, lnk) {
		if ((rt_info->entry.rt_dst == dst) &&
                ((rt_info->entry.rt_mask == mask) || (INADDR_ANY == mask)) &&
    			((rt_info->entry.rt_gateway == gw) || (INADDR_ANY == gw)) &&
    			((rt_info->entry.dev == dev) || (INADDR_ANY == dev))) {
			dlist_del_init_entry(rt_info, lnk);
			pool_free(&rt_entry_info_pool, rt_info);
			return 0;
		}
	}

	return -ENOENT;
}

/* svv: ToDo:
 *      1) this function returns -ENOENT/0, but arp_resolve -1/0
 *         style must be the same
 *      2) Carrier without ARP can't be supported
 */
int ip_route(struct sk_buff *skb, struct net_device *wanna_dev,
		struct rt_entry *suggested_route) {
	in_addr_t daddr;
	struct rt_entry *rte;

	assert(skb != NULL);
	assert(skb->nh.iph != NULL);
	daddr = skb->nh.iph->daddr;

	/* SO_BROADCAST assert. */
	if (daddr == INADDR_BROADCAST) {
		if (wanna_dev == NULL) {
			return -ENODEV;
		}
		skb->dev = wanna_dev;
		return 0;
	}

	/* if loopback set lo device */
	if (ip_is_local(daddr, false, false)) {
		assert(inetdev_get_loopback_dev() != NULL);
		skb->dev = inetdev_get_loopback_dev()->dev;
		return 0;
	}

	/* route destination address */
	rte = ((wanna_dev == NULL)
		? (suggested_route == NULL) ? rt_fib_get_best(daddr, NULL) : suggested_route
		: rt_fib_get_best(daddr, wanna_dev));
	if (rte == NULL) {
		return -ENETUNREACH;
	}

	/* set the device for current destination address */
	assert(rte->dev != NULL);
	assert((wanna_dev == NULL) || (wanna_dev == rte->dev));
	skb->dev = rte->dev;

	/* if the packet should be sent using gateway
	 * nothing todo there. all will be done in arp_resolve */
	return 0;
}

int rt_fib_route_ip(in_addr_t dst_ip, in_addr_t *next_ip) {
	struct rt_entry *rte;

	if (dst_ip == INADDR_BROADCAST) {
		*next_ip = dst_ip;
		return 0;
	}

	rte = rt_fib_get_best(dst_ip, NULL);
	if (rte == NULL) {
		return -ENETUNREACH;
	}

	*next_ip = rte->rt_gateway == INADDR_ANY ? dst_ip
			: rte->rt_gateway;

	return 0;
}

int rt_fib_source_ip(in_addr_t dst_ip, struct net_device *dev,
		in_addr_t *src_ip) {
	struct rt_entry *rte;

	if (dst_ip != INADDR_BROADCAST) {
		rte = rt_fib_get_best(dst_ip, NULL);
		if (rte == NULL) {
			return -ENETUNREACH;
		}
		assert(rte->dev != NULL);
		dev = rte->dev;
	}
	else if (dev == NULL) {
		return -ENODEV;
	}

	assert(inetdev_get_by_dev(dev) != NULL);
	*src_ip = inetdev_get_by_dev(dev)->ifa_address;

	return 0;
}

int rt_fib_out_dev(in_addr_t dst, const struct sock *sk,
		struct net_device **out_dev) {
	struct rt_entry *rte;
	struct net_device *wanna_dev;

	wanna_dev = sk != NULL ? sk->opt.so_bindtodevice : NULL;

	/* SO_BROADCAST assert. */
	if (dst == INADDR_BROADCAST) {
		if (wanna_dev == NULL) {
			return -ENODEV;
		}
		*out_dev = wanna_dev;
		return 0;
	}

	/* if loopback set lo device */
	if (ip_is_local(dst, false, false)) {
		assert(inetdev_get_loopback_dev() != NULL);
		*out_dev = inetdev_get_loopback_dev()->dev;
		return 0;
	}

	/* route destination address */
	rte = rt_fib_get_best(dst, wanna_dev);
	if (rte == NULL) {
		return -ENETUNREACH;
	}

	/* set the device for current destination address */
	assert(rte->dev != NULL);
	assert((wanna_dev == NULL) || (wanna_dev == rte->dev));
	*out_dev = rte->dev;

	/* if the packet should be sent using gateway
	 * nothing todo there. all will be done in arp_resolve */
	return 0;
}

struct rt_entry * rt_fib_get_first(void) {
	if (dlist_empty(&rt_entry_info_list)) {
		return NULL;
	}

	return &dlist_next_entry_or_null(&rt_entry_info_list,
			struct rt_entry_info, lnk)->entry;
}

struct rt_entry * rt_fib_get_next(struct rt_entry *entry) {
	struct rt_entry_info *rt_info;

	assert(entry != NULL);

	rt_info = member_cast_out(entry, struct rt_entry_info, entry);
	if (rt_info == dlist_prev_entry_or_null(&rt_entry_info_list,
			struct rt_entry_info, lnk)) {
		return NULL;
	}

	return &dlist_entry(rt_info->lnk.next,
			struct rt_entry_info, lnk)->entry;
}

/* ToDo: It's too ugly to perform sorting for every packet.
 * Routes must be added into list with mask_len decrease.
 * In this case we'll simply take the first match
 */
struct rt_entry * rt_fib_get_best(in_addr_t dst, struct net_device *out_dev) {
	struct rt_entry_info *rt_info;
	int mask_len, best_mask_len;
	struct rt_entry *best_rte;

	best_rte = NULL;
	best_mask_len = -1;
	dlist_foreach_entry(rt_info, &rt_entry_info_list, lnk) {
		mask_len = ~rt_info->entry.rt_mask
			? bit_clz(ntohl(~rt_info->entry.rt_mask)) + 1 : 32;
		if (((dst & rt_info->entry.rt_mask) == rt_info->entry.rt_dst)
				&& (out_dev == NULL || out_dev == rt_info->entry.dev)
				&& (mask_len > best_mask_len)) {
			best_rte = &rt_info->entry;
			best_mask_len = mask_len;
		}
	}

	return best_rte;
}
