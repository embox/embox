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
#include <lib/libds/bit.h>
#include <lib/libds/dlist.h>
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
	return rt_add_route_netns(dev, dst, mask, gw, flags, get_net_ns());
}

int rt_add_route_netns(struct net_device *dev, in_addr_t dst,
		in_addr_t mask, in_addr_t gw, int flags,
		net_namespace_p net_ns) {
	struct rt_entry_info *rt_info;
	bool flag = true;

	if (dev == NULL) {
		return -EINVAL;
	}

	dlist_foreach_entry(rt_info, &rt_entry_info_list, lnk) {
		if ((rt_info->entry.rt_dst == dst) &&
                ((rt_info->entry.rt_mask == mask) || (INADDR_ANY == mask)) &&
    			((rt_info->entry.rt_gateway == gw) || (INADDR_ANY == gw)) &&
			((rt_info->entry.dev == dev) || (NULL == dev)) &&
			cmp_net_ns(rt_info->entry.net_ns, net_ns)) {
			flag = false;
			return 0;
		}
	}

	if (flag) {
		rt_info = (struct rt_entry_info *)pool_alloc(&rt_entry_info_pool);
		if (rt_info == NULL) {
			return -ENOMEM;
		}
		rt_info->entry.dev = dev;
		rt_info->entry.rt_dst = dst; /* We assume that host bits are zeroes here */
		rt_info->entry.rt_mask = mask;
		rt_info->entry.rt_gateway = gw;
		rt_info->entry.rt_flags = RTF_UP | flags;
		assign_net_ns(rt_info->entry.net_ns, net_ns);
		dlist_add_prev_entry(rt_info, &rt_entry_info_list, lnk);
	}

	return 0;
}

int rt_del_route(struct net_device *dev, in_addr_t dst,
		in_addr_t mask, in_addr_t gw) {
	struct rt_entry_info *rt_info;

	dlist_foreach_entry(rt_info, &rt_entry_info_list, lnk) {
		if ((rt_info->entry.rt_dst == dst) &&
                ((rt_info->entry.rt_mask == mask) || (INADDR_ANY == mask)) &&
    			((rt_info->entry.rt_gateway == gw) || (INADDR_ANY == gw)) &&
    			((rt_info->entry.dev == dev) || (NULL == dev))) {
			dlist_del_init_entry(rt_info, lnk);
			pool_free(&rt_entry_info_pool, rt_info);
			return 0;
		}
	}

	return -ENOENT;
}

int rt_del_route_if(struct net_device *dev) {
	struct rt_entry_info *rt_info;
	int ret = 0;

	dlist_foreach_entry(rt_info, &rt_entry_info_list, lnk) {
		if (rt_info->entry.dev == dev) {
			dlist_del_init_entry(rt_info, lnk);
			pool_free(&rt_entry_info_pool, rt_info);
			ret ++;
		}
	}

	return ret ? 0 : -ENOENT;
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
	if (ip_is_local_net_ns(daddr, 0, skb->dev->net_ns)) {
		assert(inetdev_get_loopback_dev_netns(skb->dev->net_ns) != NULL);
		assign_net_ns(skb->dev->net_ns,
			inetdev_get_loopback_dev_netns(skb->dev->net_ns)->dev->net_ns);
		return 0;
	}

	/* route destination address */
	rte = ((wanna_dev == NULL)
		? (suggested_route == NULL) ?
		rt_fib_get_best_net_ns(daddr, NULL, skb->dev->net_ns) : suggested_route
		: rt_fib_get_best_net_ns(daddr, wanna_dev, skb->dev->net_ns));
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

int rt_fib_route_ip_net_ns(in_addr_t dst_ip, in_addr_t *next_ip,
					net_namespace_p net_ns) {
	struct rt_entry *rte;

	if (dst_ip == INADDR_BROADCAST) {
		*next_ip = dst_ip;
		return 0;
	}

	rte = rt_fib_get_best_net_ns(dst_ip, NULL, net_ns);
	if (rte == NULL) {
		return -ENETUNREACH;
	}

	if (rte->rt_gateway == INADDR_ANY) {
		*next_ip = dst_ip;
	} else {
		*next_ip = rte->rt_gateway;
	}

	return 0;
}

int rt_fib_source_ip_net_ns(in_addr_t dst_ip, struct net_device *dev,
		in_addr_t *src_ip, net_namespace_p net_ns) {
	struct rt_entry *rte;

	if (dst_ip != INADDR_BROADCAST) {
		rte = rt_fib_get_best_net_ns(dst_ip, NULL, net_ns);
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

int rt_fib_out_dev_net_ns(in_addr_t dst, const struct sock *sk,
		struct net_device **out_dev, net_namespace_p net_ns) {
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
	if (ip_is_local_net_ns(dst, 0, net_ns)) {
		assert(inetdev_get_loopback_dev_netns(net_ns) != NULL);
		*out_dev = inetdev_get_loopback_dev_netns(net_ns)->dev;
		return 0;
	}

	/* route destination address */
	rte = rt_fib_get_best_net_ns(dst, wanna_dev, net_ns);
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

struct rt_entry * rt_fib_get_next_net_ns(struct rt_entry *entry,
					 net_namespace_p netns) {
	struct rt_entry_info *rt_info;

	assert(entry != NULL);

	while (1) {
		rt_info = member_cast_out(entry, struct rt_entry_info, entry);
		if (rt_info == dlist_prev_entry_or_null(&rt_entry_info_list,
				struct rt_entry_info, lnk)) {
			return NULL;
		}

		entry = &dlist_entry(rt_info->lnk.next,
				struct rt_entry_info, lnk)->entry;

		if (!cmp_net_ns(entry->net_ns, netns)) {
			continue;
		} else {
			return entry;
		}
	}

	return NULL;
}

struct rt_entry * rt_fib_get_first_net_ns(net_namespace_p netns) {
	struct rt_entry * rt_entry;

	if (dlist_empty(&rt_entry_info_list)) {
		return NULL;
	}

	rt_entry = &dlist_next_entry_or_null(&rt_entry_info_list,
			struct rt_entry_info, lnk)->entry;

	if (!cmp_net_ns(rt_entry->net_ns, netns)) {
		return rt_fib_get_next_net_ns(rt_entry, netns);
	}

	return rt_entry;
}

struct rt_entry * rt_fib_get_first(void) {
	return rt_fib_get_first_net_ns(get_net_ns());
}

struct rt_entry * rt_fib_get_next(struct rt_entry *entry) {
	return rt_fib_get_next_net_ns(entry, get_net_ns());
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

struct rt_entry * rt_fib_get_best_net_ns(in_addr_t dst,
					 struct net_device *out_dev,
					 net_namespace_p net_ns) {
	struct rt_entry_info *rt_info;
	int mask_len, best_mask_len;
	struct rt_entry *best_rte;

	best_rte = NULL;
	best_mask_len = -1;
	dlist_foreach_entry(rt_info, &rt_entry_info_list, lnk) {
		if (!cmp_net_ns(rt_info->entry.net_ns, net_ns))
			continue;
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
