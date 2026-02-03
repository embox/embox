/**
 * @file
 * @brief
 *
 * @date 12.08.11
 * @author Ilia Vaprol
 */

#include <util/log.h>

#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <lib/libds/dlist.h>

#include <kernel/time/ktime.h>
#include <kernel/time/timer.h>
#include <kernel/sched/sched_lock.h>
#include <mem/misc/pool.h>

#include <net/l0/net_tx.h>
#include <net/neighbour.h>

#include <framework/mod/options.h>

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

POOL_DEF(neighbour_pool, struct neighbour, MODOPS_NEIGHBOUR_AMOUNT);
static DLIST_DEFINE(neighbour_list);
static struct sys_timer neighbour_tmr;

static void nbr_timer_handler(struct sys_timer *tmr, void *param);

static void nbr_set_haddr(struct neighbour *nbr, const void *haddr) {
	assert(nbr != NULL);

	if (haddr != NULL) {
		nbr->expire = MODOPS_NEIGHBOUR_EXPIRE;
		memcpy(&nbr->haddr[0], haddr, nbr->hlen);
	} else {
		assert(nbr->is_incomplete == 1);
	}
}

static void neighbour_timer_update(void) {
	if (dlist_empty(&neighbour_list)) {
		if (sys_timer_is_started(&neighbour_tmr)) {
			sys_timer_stop(&neighbour_tmr);
		}
	} else {
		if (!sys_timer_is_started(&neighbour_tmr)) {
			sys_timer_init_start_msec(&neighbour_tmr, SYS_TIMER_PERIODIC,
					MODOPS_NEIGHBOUR_TMR_FREQ, nbr_timer_handler, NULL);
		}
	}
}

static struct neighbour *nbr_create(unsigned short ptype, const void *paddr,
		unsigned char plen, struct net_device *dev,
		unsigned short htype, unsigned int flags ) {
	struct neighbour *nbr;

	nbr = pool_alloc(&neighbour_pool);
	if (nbr == NULL) {
		return NULL;
	}
	nbr->is_incomplete = 1;
	skb_queue_init(&nbr->w_queue);
	dlist_head_init(&nbr->lnk);
	dlist_add_prev_entry(nbr, &neighbour_list, lnk);
	nbr->ptype = ptype;
	memcpy(nbr->paddr, paddr, plen);
	nbr->plen = plen;
	nbr->dev = dev;
	nbr->htype = htype;
	nbr->hlen = dev->addr_len;
	nbr_set_haddr(nbr, NULL);
	nbr->resend = MODOPS_NEIGHBOUR_RESEND;
	nbr->sent_times = 0;

	neighbour_timer_update();

	return nbr;
}

static void nbr_free(struct neighbour *nbr) {
	assert(nbr != NULL);

	dlist_del_init_entry(nbr, lnk);
	skb_queue_purge(&nbr->w_queue);
	pool_free(&neighbour_pool, nbr);

	neighbour_timer_update();
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

	++nbr->sent_times;

	if (nbr->ptype == ETH_P_IP) {
		in_dev = inetdev_get_by_dev(nbr->dev);
		assert(in_dev != NULL);
		return arp_discover(nbr->dev, nbr->ptype, nbr->plen,
				&in_dev->ifa_address, &nbr->paddr[0]);
	} else {
		assert(nbr->ptype == ETH_P_IPV6);
		return ndp_discover(nbr->dev, &nbr->paddr[0]);
	}
}

static int nbr_build_and_send_pkt(struct sk_buff *skb, struct neighbour *nbr) {
	int ret;
	struct net_header_info hdr_info;

	hdr_info.type = nbr->ptype;
	hdr_info.src_hw = &nbr->dev->dev_addr[0];
	hdr_info.dst_hw = &nbr->haddr[0];

	assert(skb != NULL);

	/* try to rebuild */
	assert(skb->dev != NULL);
	assert(skb->dev->ops != NULL);
	assert(skb->dev->ops->build_hdr != NULL);
	ret = skb->dev->ops->build_hdr(skb, &hdr_info);
	if (ret) {
		skb_free(skb);
		return ret;
	}

	/* try to xmit */
	ret = net_tx_direct(skb);

	return ret;
}

static void nbr_flush_w_queue(struct neighbour *nbr) {
	struct sk_buff *skb;

	while ((skb = skb_queue_pop(&nbr->w_queue)) != NULL) {
		(void)nbr_build_and_send_pkt(skb, nbr);
	}
}

int neighbour_add(unsigned short ptype, const void *paddr,
		unsigned char plen, struct net_device *dev,
		unsigned short htype, const void *haddr, unsigned char hlen,
		unsigned int flags) {
	int ret;
	struct neighbour *nbr;

	if ((paddr == NULL) || (plen == 0) || (plen > sizeof(nbr->paddr))
			|| (dev == NULL) || (haddr == NULL) || (hlen == 0)
			|| hlen < dev->addr_len	|| (hlen > sizeof(nbr->haddr))) {
		return -EINVAL;
	}

	ret = 0;
	sched_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr == NULL) {
			nbr = nbr_create(ptype, paddr, plen, dev, htype, flags);
			if (nbr == NULL) {
				ret = - ENOMEM;
				goto exit;
			}
		}

		nbr_set_haddr(nbr, haddr);
		nbr->flags = flags;

		if (nbr->is_incomplete) {
			nbr_flush_w_queue(nbr);
			nbr->is_incomplete = 0;
		}
	}
exit:
	sched_unlock();

	return ret;
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
		} else if (nbr->htype != htype) {
			sched_unlock();
			return -ENOENT;
		} else if (nbr->is_incomplete) {
			sched_unlock();
			return -EINPROGRESS;
		} else if (nbr->hlen > hlen_max) {
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
	int ret;

	if ((paddr == NULL) || (dev == NULL)) {
		return -EINVAL;
	}

	sched_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr != NULL) {
			nbr_free(nbr);
			ret = 0;
		} else {
			ret = -ENOENT;
		}
	}
	sched_unlock();

	return ret;
}

int neighbour_clean(struct net_device *dev) {
	struct neighbour *nbr = NULL;

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

#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
#include <net/net_namespace.h>

int neighbour_foreach_net_ns(neighbour_foreach_ft func, void *args,
			net_namespace_p net_ns) {
	int ret;
	struct neighbour *nbr;

	if (func == NULL) {
		return -EINVAL;
	}

	sched_lock();
	{
		dlist_foreach_entry(nbr, &neighbour_list, lnk) {
			if (!cmp_net_ns(nbr->dev->net_ns, net_ns))
				continue;
			sched_unlock();

			ret = (*func)(nbr, args);
			if (ret != 0) {
				return ret;
			}

			sched_lock();
		}
	}
	sched_unlock();

	return 0;
}

int neighbour_foreach(neighbour_foreach_ft func, void *args) {
	return neighbour_foreach_net_ns(func, args, get_net_ns());
}

#else

int neighbour_foreach(neighbour_foreach_ft func, void *args) {
	int ret;
	struct neighbour *nbr = NULL;

	if (func == NULL) {
		return -EINVAL;
	}

	sched_lock();
	{
		dlist_foreach_entry(nbr, &neighbour_list, lnk) {
			sched_unlock();

			ret = (*func)(nbr, args);
			if (ret != 0) {
				return ret;
			}

			sched_lock();
		}
	}
	sched_unlock();

	return 0;
}
#endif

int neighbour_resolve(unsigned short ptype,
		const void *paddr, unsigned char plen,
		struct net_device *dev, struct sk_buff *skb,
		unsigned char hlen_max, void *out_haddr) {
	struct neighbour *nbr;
	int ret;

	if (hlen_max < dev->addr_len) {
		return -EINVAL;
	}

	ret = 0;

	sched_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr == NULL) {
			nbr = nbr_create(ptype, paddr, plen, dev, dev->type, 0);
			if (nbr == NULL) {
				ret = -ENOMEM;
				goto exit;
			}

			nbr_send_request(nbr);
		}

		if (nbr->is_incomplete) {
			skb_queue_push(&nbr->w_queue, skb);
			ret = EHOSTUNREACH;
			goto exit;
		}

		memcpy(out_haddr, &nbr->haddr[0], nbr->hlen);
	}
exit:
	sched_unlock();

	return ret;
}

static void nbr_timer_handler(struct sys_timer *tmr, void *param) {
	struct neighbour *nbr = NULL;

	sched_lock();
	{
		dlist_foreach_entry(nbr, &neighbour_list, lnk) {
			if (nbr->flags & NEIGHBOUR_FLAG_PERMANENT) {
				continue;
			}

			if (nbr->is_incomplete) {
				if (--nbr->expire <= 0) {
					nbr_free(nbr);
				}
			} else {
				/* have not revolved yet */
				if ( --nbr->resend <= 0) {
					if (nbr->sent_times++ == MODOPS_NEIGHBOUR_ATTEMPT) {
						nbr_free(nbr);
						/* unreachable host */
					} else {
						(void)nbr_send_request(nbr);
						nbr->resend = MODOPS_NEIGHBOUR_RESEND;
					}
				}
			}
		}
	}
	sched_unlock();
}
