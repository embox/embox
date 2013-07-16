/**
 * @file
 * @brief
 *
 * @date 12.08.11
 * @author Ilia Vaprol
 */

#include <net/neighbour.h>
#include <errno.h>
#include <kernel/irq_lock.h>
#include <mem/misc/pool.h>
#include <string.h>
#include <util/dlist.h>
#include <time.h>
#include <util/list.h>
#include <util/array.h>
#include <sys/time.h>
#include <kernel/time/ktime.h>
#include <kernel/time/timer.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

#include <net/l3/arp.h>
#include <kernel/printk.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>

#define MODOPS_NEIGHBOUR_AMOUNT   OPTION_GET(NUMBER, neighbour_amount)
#define MODOPS_NEIGHBOUR_EXPIRE   OPTION_GET(NUMBER, neighbour_expire)
#define MODOPS_NEIGHBOUR_TMR_FREQ OPTION_GET(NUMBER, neighbour_tmr_freq)
#define MODOPS_NEIGHBOUR_RESEND   OPTION_GET(NUMBER, neighbour_resend)

EMBOX_UNIT_INIT(neighbour_init);

POOL_DEF(neighbour_pool, struct neighbour, MODOPS_NEIGHBOUR_AMOUNT);
static LIST_DEF(neighbour_list);
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
	}
}

static void nbr_free(struct neighbour *nbr) {
	assert(nbr != NULL);

	list_unlink_element(nbr, lnk);
	skb_queue_purge(&nbr->w_queue);
	pool_free(&neighbour_pool, nbr);
}

static struct neighbour * nbr_lookup_by_paddr(unsigned short ptype,
		const void *paddr, struct net_device *dev) {
	struct neighbour *nbr;

	assert(paddr != NULL);
	assert(dev != NULL);

	list_foreach(nbr, &neighbour_list, lnk) {
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

	list_foreach(nbr, &neighbour_list, lnk) {
		if ((nbr->htype == htype)
				&& (0 == memcmp(&nbr->haddr[0], haddr, nbr->hlen))
				&& (nbr->dev == dev)) {
			return nbr;
		}
	}

	return NULL; /* error: no such entity */
}

static int nbr_send_request(struct neighbour *nbr) {
	in_addr_t saddr;
	struct in_device *in_dev;

	in_dev = inetdev_get_by_dev(nbr->dev);
	assert(in_dev != NULL);

	saddr = in_dev->ifa_address;

	return arp_send(ARP_OPER_REQUEST, nbr->ptype, nbr->hlen,
			nbr->plen, NULL, &saddr, NULL, &nbr->paddr[0], NULL, nbr->dev);
}

static void nbr_flush_w_queue(struct neighbour *nbr) {
	struct sk_buff *skb;

	assert(nbr != NULL);

	while ((skb = skb_queue_pop(&nbr->w_queue)) != NULL) {
		/* try to rebuild */
		assert(nbr->dev != NULL);
		assert(nbr->dev->ops != NULL);
		assert(nbr->dev->ops->rebuild_hdr != NULL);
		if (0 == nbr->dev->ops->rebuild_hdr(skb)) {
			/* try to xmit */
			if (0 != dev_xmit_skb(skb)) {
				printk("nbr_flush_w_queue: erorr: can't xmit over device\n");
			}
		}
		else {
			printk("nbr_flush_w_queue: error: can't rebuild after resolving\n");
			skb_free(skb);
		}
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

	irq_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		exist = nbr != NULL;
		if (nbr == NULL) {
			nbr = pool_alloc(&neighbour_pool);
		}
	}
	irq_unlock();
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
		list_link_init(&nbr->lnk);
		nbr->ptype = ptype;
		memcpy(nbr->paddr, paddr, plen);
		nbr->plen = plen;
		nbr->dev = dev;
		skb_queue_init(&nbr->w_queue);

		irq_lock();
		{
			list_add_last_element(nbr, &neighbour_list, lnk);
		}
		irq_unlock();
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

	irq_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr == NULL) {
			irq_unlock();
			return -ENOENT;
		}
		else if (nbr->htype != htype) {
			irq_unlock();
			return -ENOENT;
		}
		else if (nbr->incomplete) {
			return -EINPROGRESS;
		}
		else if (nbr->hlen > hlen_max) {
			irq_unlock();
			return -ENOMEM;
		}

		memcpy(out_haddr, &nbr->haddr[0], nbr->hlen);
	}
	irq_unlock();

	return 0;
}

int neighbour_get_paddr(unsigned short htype, const void *haddr,
		struct net_device *dev, unsigned short ptype,
		unsigned char plen_max, void *out_paddr) {
	struct neighbour *nbr;

	if ((haddr == NULL) || (dev == NULL) || (out_paddr == NULL)) {
		return -EINVAL;
	}

	irq_lock();
	{
		nbr = nbr_lookup_by_haddr(htype, haddr, dev);
		if (nbr == NULL) {
			irq_unlock();
			return -ENOENT;
		}
		else if (nbr->ptype != ptype) {
			irq_unlock();
			return -ENOENT;
		}
		else if (nbr->plen > plen_max) {
			irq_unlock();
			return -ENOMEM;
		}

		memcpy(out_paddr, &nbr->paddr[0], nbr->plen);
	}
	irq_unlock();

	return 0;
}

int neighbour_del(unsigned short ptype, const void *paddr,
		struct net_device *dev) {
	struct neighbour *nbr;

	if ((paddr == NULL) || (dev == NULL)) {
		return -EINVAL;
	}

	irq_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr == NULL) {
			irq_unlock();
			return -ENOENT;
		}

		nbr_free(nbr);
	}
	irq_unlock();

	return 0;
}

int neighbour_clean(struct net_device *dev) {
	struct neighbour *nbr;

	irq_lock();
	{
		list_foreach(nbr, &neighbour_list, lnk) {
			if ((nbr->dev == dev) || (dev == NULL)) {
				nbr_free(nbr);
			}
		}
	}
	irq_unlock();

	return 0;
}

int neighbour_foreach(neighbour_foreach_ft func, void *args) {
	int ret;
	struct neighbour *nbr;

	if (func == NULL) {
		return -EINVAL;
	}

	irq_lock();
	{
		list_foreach(nbr, &neighbour_list, lnk) {
			ret = (*func)(nbr, args);
			if (ret != 0) {
				irq_unlock();
				return ret;
			}
		}
	}
	irq_unlock();

	return 0;
}

int neighbour_send_after_resolve(unsigned short ptype,
		const void *paddr, unsigned char plen,
		struct net_device *dev, struct sk_buff *skb) {
	int resolved;
	struct neighbour *nbr;

	if ((paddr == NULL) || (dev == NULL) || (dev == NULL)) {
		return -EINVAL;
	}

	irq_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr == NULL) {
			nbr = pool_alloc(&neighbour_pool);
			if (nbr == NULL) {
				irq_unlock();
				return -ENOMEM;
			}
			list_link_init(&nbr->lnk);
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

			list_add_last_element(nbr, &neighbour_list, lnk);
		}

		resolved = !nbr->incomplete;
		if (nbr->incomplete) {
			skb_queue_push(&nbr->w_queue, skb);
		}
	}
	irq_unlock();

	if (resolved) {
		return dev_send_skb(skb);
	}

	return nbr_send_request(nbr);
}

static void nbr_timer_handler(struct sys_timer *tmr, void *param) {
	struct neighbour *nbr;

	irq_lock();
	{
		list_foreach(nbr, &neighbour_list, lnk) {
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
					nbr_send_request(nbr);
					nbr->resend = MODOPS_NEIGHBOUR_RESEND;
				}
				else {
					nbr->resend -= MODOPS_NEIGHBOUR_TMR_FREQ;
				}
			}
		}
	}
	irq_unlock();
}

static int neighbour_init(void) {
	int ret;

	ret = timer_init(&neighbour_tmr, TIMER_PERIODIC,
			MODOPS_NEIGHBOUR_TMR_FREQ, nbr_timer_handler, NULL);
	if (ret != 0) {
		return ret;
	}

	return 0;
}
