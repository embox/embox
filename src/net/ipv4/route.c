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
#include <util/member.h>
#include <mem/misc/pool.h>
#include <lib/list.h>

/**
 * NOTE: Linux route uses 3 structs for routing:
 *    + Forwarding Information Base (FIB)
 *    - routing cache (256 chains)
 *    + neighbour table (ARP cache)
 */

struct rt_entry_info {
	struct list_head lnk;
	struct rt_entry entry;
};

POOL_DEF(rt_entry_info_pool, struct rt_entry_info, CONFIG_ROUTE_FIB_TABLE_SIZE);
static struct list_head rt_entry_info_list;

int rt_add_route(net_device_t *dev, in_addr_t dst,
			in_addr_t mask, in_addr_t gw, int flags) {
	struct rt_entry_info *rt_info;

	rt_info = (struct rt_entry_info *)pool_alloc(&rt_entry_info_pool);
	if (rt_info == NULL) {
		return -ENOMEM;
	}
	rt_info->entry.dev = dev;
	rt_info->entry.rt_dst = dst;
	rt_info->entry.rt_mask = mask;
	rt_info->entry.rt_gateway = gw;
	rt_info->entry.rt_flags = RTF_UP | flags;

	list_add_tail(&rt_info->lnk, &rt_entry_info_list);

	return ENOERR;
}

int rt_del_route(net_device_t *dev, in_addr_t dst,
			    in_addr_t mask, in_addr_t gw) {
	struct rt_entry_info *rt_info;
	struct list_head *tmp;

	list_for_each(tmp, &rt_entry_info_list) {
		rt_info = member_cast_out(tmp, struct rt_entry_info, lnk);
		if (((rt_info->entry.rt_dst == dst) || (INADDR_ANY == dst)) &&
				((rt_info->entry.rt_mask == mask) || (INADDR_ANY == mask)) &&
				((rt_info->entry.rt_gateway == gw) || (INADDR_ANY == gw))) {
			list_del(&rt_info->lnk);
			pool_free(&rt_entry_info_pool, rt_info);
			return ENOERR;
		}
	}

	return -ENOENT;
}

int ip_route(sk_buff_t *skb) {
	struct rt_entry_info *rt_info;
	struct list_head *tmp;
	in_addr_t daddr;
	int arp_resolve_result;

	daddr = skb->nh.iph->daddr;
	list_for_each(tmp, &rt_entry_info_list) {
		rt_info = member_cast_out(tmp, struct rt_entry_info, lnk);
		if ((daddr & rt_info->entry.rt_mask) == rt_info->entry.rt_dst) {
			/* set the device for current destination address */
			skb->dev = rt_info->entry.dev;
			// TODO even if type is SOCK_RAW?
			/* set the source address */
			skb->nh.iph->saddr = in_dev_get(skb->dev)->ifa_address;
			/* if source and destination addresses are equal send via LB interface */
			if(skb->nh.iph->daddr  == skb->nh.iph->saddr)
				skb->dev = inet_get_loopback_dev();
			/* if the packet should be sent using gateway */
			if (rt_info->entry.rt_gateway != INADDR_ANY) {
				/* the next line coerses arp_resolve to set HW destination address
					 to gateway's HW address*/
				skb->nh.iph->daddr = rt_info->entry.rt_gateway;
				arp_resolve_result = arp_resolve(skb);
				/* so here we need to return the original destination address */
				skb->nh.iph->daddr = daddr;
				return arp_resolve_result;
			}
			return ENOERR;
		}
	}

	return -ENOENT;
}

struct rt_entry * rt_fib_get_first(void) {
	struct rt_entry_info *rt_info;

	if (list_empty(&rt_entry_info_list)) {
		return NULL;
	}

	rt_info = member_cast_out(rt_entry_info_list.next, struct rt_entry_info, lnk);

	return &rt_info->entry;
}

struct rt_entry * rt_fib_get_next(struct rt_entry *entry) {
	struct rt_entry_info *rt_info;

	rt_info = member_cast_out(entry, struct rt_entry_info, entry);

	rt_info = member_cast_out(rt_info->lnk.next, struct rt_entry_info, lnk);

	return (&rt_info->lnk == &rt_entry_info_list) ? NULL : &rt_info->entry;
}

int route_init(void) {
	INIT_LIST_HEAD(&rt_entry_info_list);
	return ENOERR;
}
