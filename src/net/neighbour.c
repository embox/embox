/**
 * @file
 * @brief
 *
 * @date 12.08.11
 * @author Ilia Vaprol
 */

#include <net/neighbour.h>
#include <errno.h>
#include <kernel/sched/sched_lock.h>
#include <mem/misc/pool.h>
#include <string.h>
#include <time.h>
#include <util/dlist.h>
#include <util/array.h>
#include <sys/time.h>
#include <kernel/time/ktime.h>
#include <kernel/time/timer.h>
#include <net/l0/net_tx.h>
#include <util/binalign.h>
#include <util/log.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

#include <net/l3/arp.h>
#include <net/l3/ndp.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>

#define MODOPS_NEIGHBOUR_AMOUNT   OPTION_GET(NUMBER, neighbour_amount)
#define MODOPS_NEIGHBOUR_EXPIRE   OPTION_GET(NUMBER, neighbour_expire)
#define MODOPS_NEIGHBOUR_TMR_FREQ OPTION_GET(NUMBER, neighbour_tmr_freq)
#define MODOPS_NEIGHBOUR_RESEND   OPTION_GET(NUMBER, neighbour_resend)
#define MODOPS_NEIGHBOUR_ATTEMPT  OPTION_GET(NUMBER, neighbour_attempt)

EMBOX_UNIT_INIT(neighbour_init);

POOL_DEF(neighbour_pool, struct neighbour, MODOPS_NEIGHBOUR_AMOUNT);
static DLIST_DEFINE(neighbour_list);
static struct sys_timer neighbour_tmr;

static void nbr_set_haddr(struct neighbour *nbr, const void *haddr) {
	assert(nbr != NULL);

	if (haddr != NULL) {
		nbr->incomplete = 0;
		memcpy(&nbr->haddr[0], haddr, nbr->hlen);
	}
	else {
		nbr->incomplete = 1;
		nbr->resend = MODOPS_NEIGHBOUR_RESEND;
		nbr->sent_times = 0;
	}
}

static void nbr_free(struct neighbour *nbr) {
	assert(nbr != NULL);

	dlist_del_init_entry(nbr, lnk);
	skb_queue_purge(&nbr->w_queue);
	pool_free(&neighbour_pool, nbr);
}

static struct neighbour * nbr_lookup_by_paddr(unsigned short ptype,
		const void *paddr, struct net_device *dev) {
	struct neighbour *nbr;

	assert(paddr != NULL);
	assert(dev != NULL);

	dlist_foreach_entry(nbr, &neighbour_list, lnk) {
		if ((nbr->ptype == ptype)
				&& (0 == memcmp(&nbr->paddr[0], paddr, nbr->plen))
				&& (nbr->dev == dev)) {
			return nbr;
		}
	}

	return NULL; /* error: no such entity */
}

static struct neighbour * nbr_lookup_by_haddr(unsigned short htype,
		const void *haddr, struct net_device *dev) {
	struct neighbour *nbr;

	assert(haddr != NULL);
	assert(dev != NULL);

	dlist_foreach_entry(nbr, &neighbour_list, lnk) {
		if ((nbr->htype == htype)
				&& (0 == memcmp(&nbr->haddr[0], haddr, nbr->hlen))
				&& (nbr->dev == dev)) {
			return nbr;
		}
	}

	return NULL; /* error: no such entity */
}

static int nbr_send_request(struct neighbour *nbr) {
	struct in_device *in_dev;
	struct {
		struct ndpbody_neighbor_solicit body;
		struct ndpoptions_ll_addr ops;
		char __ops_ll_addr_storage[MAX_ADDR_LEN];
	} __attribute__((packed)) nbr_solicit;

	++nbr->sent_times;

	if (nbr->ptype == ETH_P_IP) {
		in_dev = inetdev_get_by_dev(nbr->dev);
		assert(in_dev != NULL);
		return arp_discover(nbr->dev, nbr->ptype, nbr->plen,
				&in_dev->ifa_address, &nbr->paddr[0]);
	}
	else {
		assert(nbr->ptype == ETH_P_IPV6);
		nbr_solicit.body.zero = 0;
		memcpy(&nbr_solicit.body.target, &nbr->paddr[0],
				sizeof nbr_solicit.body.target);
		nbr_solicit.ops.hdr.type = NDP_SOURCE_LL_ADDR;
		nbr_solicit.ops.hdr.len = binalign_bound(sizeof nbr_solicit.ops
				+ nbr->dev->addr_len, 8) / 8;
		memcpy(nbr_solicit.ops.ll_addr, &nbr->dev->dev_addr[0],
				nbr->dev->addr_len);
		return ndp_send(NDP_NEIGHBOR_SOLICIT, 0, &nbr_solicit,
				sizeof nbr_solicit.body + sizeof nbr_solicit.ops
					+ nbr->dev->addr_len, nbr->dev);
	}
}

static int nbr_build_and_send_pkt(struct sk_buff *skb,
		const struct net_header_info *hdr_info) {
	int ret;

	assert(skb != NULL);
	assert(hdr_info != NULL);

	/* try to rebuild */
	assert(skb->dev != NULL);
	assert(skb->dev->ops != NULL);
	assert(skb->dev->ops->build_hdr != NULL);
	ret = skb->dev->ops->build_hdr(skb, hdr_info);
	if (ret == 0) {
		/* try to xmit */
		ret = net_tx(skb, NULL);
		if (ret != 0) {
			log_error("nbr_build_and_send_pkt: error: can't xmit over device, code %d\n", ret);
			return ret;
		}
	}
	else {
		log_error("nbr_build_and_send_pkt: error: can't build after resolving, code %d\n", ret);
		skb_free(skb);
		return ret;
	}

	return 0;
}

static void nbr_flush_w_queue(struct neighbour *nbr) {
	struct sk_buff *skb;
	struct net_header_info hdr_info;

	hdr_info.type = nbr->ptype;
	hdr_info.src_hw = &nbr->dev->dev_addr[0];
	hdr_info.dst_hw = &nbr->haddr[0];

	while ((skb = skb_queue_pop(&nbr->w_queue)) != NULL) {
		(void)nbr_build_and_send_pkt(skb, &hdr_info);
	}
}

int neighbour_add(unsigned short ptype, const void *paddr,
		unsigned char plen, struct net_device *dev,
		unsigned short htype, const void *haddr, unsigned char hlen,
		unsigned int flags) {
	int exist;
	struct neighbour *nbr;

	if ((paddr == NULL) || (plen == 0)
			|| (plen > ARRAY_SIZE(nbr->paddr)) || (dev == NULL)
			|| (haddr == NULL) || (hlen == 0)
			|| (hlen > ARRAY_SIZE(nbr->haddr))) {
		return -EINVAL;
	}

	sched_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		exist = nbr != NULL;
		if (nbr == NULL) {
			nbr = pool_alloc(&neighbour_pool);
		}
	}
	sched_unlock();
	if (nbr == NULL) {
		return -ENOMEM;
	}

	nbr->htype = htype;
	nbr->hlen = hlen;
	nbr_set_haddr(nbr, haddr);
	nbr->flags = flags;
	nbr->expire = MODOPS_NEIGHBOUR_EXPIRE;

	if (exist) {
		nbr_flush_w_queue(nbr);
	}
	else {
		dlist_head_init(&nbr->lnk);
		nbr->ptype = ptype;
		memcpy(nbr->paddr, paddr, plen);
		nbr->plen = plen;
		nbr->dev = dev;
		skb_queue_init(&nbr->w_queue);

		sched_lock();
		{
			dlist_add_prev_entry(nbr, &neighbour_list, lnk);
		}
		sched_unlock();
	}

	return 0;
}

int neighbour_get_haddr(unsigned short ptype,  const void *paddr,
		struct net_device *dev, unsigned short htype,
		unsigned char hlen_max, void *out_haddr) {
	struct neighbour *nbr;

	if ((paddr == NULL) || (dev == NULL) || (out_haddr == NULL)) {
		return -EINVAL;
	}

	sched_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr == NULL) {
			sched_unlock();
			return -ENOENT;
		}
		else if (nbr->htype != htype) {
			sched_unlock();
			return -ENOENT;
		}
		else if (nbr->incomplete) {
			sched_unlock();
			return -EINPROGRESS;
		}
		else if (nbr->hlen > hlen_max) {
			sched_unlock();
			return -ENOMEM;
		}

		memcpy(out_haddr, &nbr->haddr[0], nbr->hlen);
	}
	sched_unlock();

	return 0;
}

int neighbour_get_paddr(unsigned short htype, const void *haddr,
		struct net_device *dev, unsigned short ptype,
		unsigned char plen_max, void *out_paddr) {
	struct neighbour *nbr;

	if ((haddr == NULL) || (dev == NULL) || (out_paddr == NULL)) {
		return -EINVAL;
	}

	sched_lock();
	{
		nbr = nbr_lookup_by_haddr(htype, haddr, dev);
		if (nbr == NULL) {
			sched_unlock();
			return -ENOENT;
		}
		else if (nbr->ptype != ptype) {
			sched_unlock();
			return -ENOENT;
		}
		else if (nbr->plen > plen_max) {
			sched_unlock();
			return -ENOMEM;
		}

		memcpy(out_paddr, &nbr->paddr[0], nbr->plen);
	}
	sched_unlock();

	return 0;
}

int neighbour_del(unsigned short ptype, const void *paddr,
		struct net_device *dev) {
	struct neighbour *nbr;

	if ((paddr == NULL) || (dev == NULL)) {
		return -EINVAL;
	}

	sched_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr == NULL) {
			sched_unlock();
			return -ENOENT;
		}

		nbr_free(nbr);
	}
	sched_unlock();

	return 0;
}

int neighbour_clean(struct net_device *dev) {
	struct neighbour *nbr;

	sched_lock();
	{
		dlist_foreach_entry(nbr, &neighbour_list, lnk) {
			if ((nbr->dev == dev) || (dev == NULL)) {
				nbr_free(nbr);
			}
		}
	}
	sched_unlock();

	return 0;
}

int neighbour_foreach(neighbour_foreach_ft func, void *args) {
	int ret;
	struct neighbour *nbr;

	if (func == NULL) {
		return -EINVAL;
	}

	sched_lock();
	{
		dlist_foreach_entry(nbr, &neighbour_list, lnk) {
			sched_unlock();
			ret = (*func)(nbr, args);
			sched_lock();
			if (ret != 0) {
				sched_unlock();
				return ret;
			}
		}
	}
	sched_unlock();

	return 0;
}

int neighbour_send_after_resolve(unsigned short ptype,
		const void *paddr, unsigned char plen,
		struct net_device *dev, struct sk_buff *skb) {
	int allocated, resolved;
	struct neighbour *nbr;
	struct net_header_info hdr_info;

	if ((paddr == NULL) || (dev == NULL) || (dev == NULL)) {
		skb_free(skb);
		return -EINVAL;
	}

	sched_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr == NULL) {
			nbr = pool_alloc(&neighbour_pool);
			if (nbr == NULL) {
				sched_unlock();
				skb_free(skb);
				return -ENOMEM;
			}
			dlist_head_init(&nbr->lnk);
			nbr->ptype = ptype;
			memcpy(nbr->paddr, paddr, plen);
			nbr->plen = plen;
			nbr->dev = dev;
			nbr->htype = dev->type;
			nbr->hlen = dev->addr_len;
			nbr_set_haddr(nbr, NULL);
			nbr->flags = 0;
			skb_queue_init(&nbr->w_queue);
			nbr->expire = MODOPS_NEIGHBOUR_EXPIRE;

			dlist_add_prev_entry(nbr, &neighbour_list, lnk);

			allocated = 1;
		}
		else {
			allocated = 0;
		}

		resolved = !nbr->incomplete;

		if (!resolved) {
			skb_queue_push(&nbr->w_queue, skb);
		}
	}
	sched_unlock();

	if (resolved) {
		hdr_info.type = nbr->ptype;
		hdr_info.src_hw = &nbr->dev->dev_addr[0];
		hdr_info.dst_hw = &nbr->haddr[0];
		return nbr_build_and_send_pkt(skb, &hdr_info);
	}

	if (allocated) {
		(void)nbr_send_request(nbr);
	}

	return 0;
}

static void nbr_timer_handler(struct sys_timer *tmr, void *param) {
	struct neighbour *nbr;

	sched_lock();
	{
		dlist_foreach_entry(nbr, &neighbour_list, lnk) {
			if (nbr->flags & NEIGHBOUR_FLAG_PERMANENT) {
				continue;
			}
			else if (nbr->expire <= MODOPS_NEIGHBOUR_TMR_FREQ) {
				nbr_free(nbr);
				continue;
			}

			nbr->expire -= MODOPS_NEIGHBOUR_TMR_FREQ;

			if (nbr->incomplete) {
				if (nbr->resend <= MODOPS_NEIGHBOUR_TMR_FREQ) {
					if (nbr->sent_times == MODOPS_NEIGHBOUR_ATTEMPT) {
						nbr_free(nbr);
					}
					else {
						(void)nbr_send_request(nbr);
						nbr->resend = MODOPS_NEIGHBOUR_RESEND;
					}
				}
				else {
					nbr->resend -= MODOPS_NEIGHBOUR_TMR_FREQ;
				}
			}
		}
	}
	sched_unlock();
}

static int neighbour_init(void) {
	int ret;

	ret = timer_init_start_msec(&neighbour_tmr, TIMER_PERIODIC,
			MODOPS_NEIGHBOUR_TMR_FREQ, nbr_timer_handler, NULL);
	if (ret != 0) {
		return ret;
	}

	return 0;
}
