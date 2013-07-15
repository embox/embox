/**
 * @file
 * @brief
 *
 * @date 12.08.11
 * @author Ilia Vaprol
 */

#include <net/neighbour.h>
#include <errno.h>
#include <kernel/softirq_lock.h>
#include <mem/misc/pool.h>
#include <string.h>
#include <util/dlist.h>
#include <time.h>
#include <util/list.h>
#include <util/array.h>
#include <sys/time.h>
#include <kernel/time/ktime.h>

#include <framework/mod/options.h>

#define MODOPS_NEIGHBOUR_AMOUNT OPTION_GET(NUMBER, neighbour_amount)
#define MODOPS_NEIGHBOUR_EXPIRE OPTION_GET(NUMBER, neighbour_expire)

POOL_DEF(neighbour_pool, struct neighbour, MODOPS_NEIGHBOUR_AMOUNT);
static LIST_DEF(neighbour_list);

#define NEIGHBOUR_CHECK_EXPIRE(nbr, now)                       \
	do {                                                       \
		if (timercmp(now, &nbr->expire, >=)                    \
				&& !(nbr->flags & NEIGHBOUR_FLAG_PERMANENT)) { \
			nbr_free(nbr);                                     \
			continue;                                          \
		}                                                      \
	} while (0)

static void nbr_set_haddr(struct neighbour *nbr, const void *haddr) {
	assert(nbr != NULL);

	nbr->incomplete = haddr == NULL;
	if (haddr != NULL) {
		memcpy(&nbr->haddr[0], haddr, nbr->hlen);
	}
}

static void nbr_set_expire(struct neighbour *nbr) {
	struct timeval now;

	assert(nbr != NULL);

	ktime_get_timeval(&now);
	ms_to_timeval(MODOPS_NEIGHBOUR_EXPIRE, &nbr->expire);
	timeradd(&nbr->expire, &now, &nbr->expire);
}

static void nbr_free(struct neighbour *nbr) {
	assert(nbr != NULL);

	list_unlink_element(nbr, lnk);
	skb_queue_purge(&nbr->w_queue);
	pool_free(&neighbour_pool, nbr);
}

static struct neighbour * nbr_lookup_by_paddr(unsigned short ptype,
		const void *paddr, const struct net_device *dev) {
	struct neighbour *nbr;
	struct timeval now;

	assert(paddr != NULL);
	assert(dev != NULL);

	ktime_get_timeval(&now);

	list_foreach(nbr, &neighbour_list, lnk) {
		NEIGHBOUR_CHECK_EXPIRE(nbr, &now);
		if ((nbr->ptype == ptype)
				&& (0 == memcmp(&nbr->paddr[0], paddr, nbr->plen))
				&& (nbr->dev == dev)) {
			return nbr;
		}
	}

	return NULL; /* error: no such entity */
}

static struct neighbour * nbr_lookup_by_haddr(unsigned short htype,
		const void *haddr, const struct net_device *dev) {
	struct neighbour *nbr;
	struct timeval now;

	assert(haddr != NULL);
	assert(dev != NULL);

	ktime_get_timeval(&now);

	list_foreach(nbr, &neighbour_list, lnk) {
		NEIGHBOUR_CHECK_EXPIRE(nbr, &now);
		if ((nbr->htype == htype)
				&& (0 == memcmp(&nbr->haddr[0], haddr, nbr->hlen))
				&& (nbr->dev == dev)) {
			return nbr;
		}
	}

	return NULL; /* error: no such entity */
}

int neighbour_add(unsigned short ptype, const void *paddr,
		unsigned char plen, const struct net_device *dev,
		unsigned short htype, const void *haddr, unsigned char hlen,
		unsigned int flags) {
	struct neighbour *nbr;

	if ((paddr == NULL) || (plen == 0)
			|| (plen > ARRAY_SIZE(nbr->paddr))
			|| (dev == NULL) || (hlen == 0)
			|| (hlen > ARRAY_SIZE(nbr->haddr))) {
		return -EINVAL;
	}

	softirq_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr != NULL) {
			softirq_unlock();
			return -EEXIST;
		}

		nbr = pool_alloc(&neighbour_pool);
		if (nbr == NULL) {
			softirq_unlock();
			return -ENOMEM;
		}
	}
	softirq_unlock();

	list_link_init(&nbr->lnk);
	nbr->ptype = ptype;
	memcpy(nbr->paddr, paddr, plen);
	nbr->plen = plen;
	nbr->dev = dev;
	nbr->htype = htype;
	nbr->hlen = hlen;
	nbr_set_haddr(nbr, haddr);
	nbr->flags = flags;
	nbr_set_expire(nbr);
	skb_queue_init(&nbr->w_queue);

	softirq_lock();
	{
		list_add_last_element(nbr, &neighbour_list, lnk);
	}
	softirq_unlock();

	return 0;
}

int neighbour_set_haddr(unsigned short ptype,
		const void *paddr, const struct net_device *dev,
		const void *haddr) {
	struct neighbour *nbr;

	if ((paddr == NULL) || (dev == NULL)) {
		return -EINVAL;
	}

	softirq_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr == NULL) {
			softirq_unlock();
			return -ENOENT;
		}

		nbr_set_haddr(nbr, haddr);
		nbr_set_expire(nbr);
	}
	softirq_unlock();

	return 0;
}

int neighbour_get_haddr(unsigned short ptype,  const void *paddr,
		const struct net_device *dev, unsigned short htype,
		unsigned char hlen_max, void *out_haddr) {
	struct neighbour *nbr;

	if ((paddr == NULL) || (dev == NULL) || (out_haddr == NULL)) {
		return -EINVAL;
	}

	softirq_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr == NULL) {
			softirq_unlock();
			return -ENOENT;
		}
		else if (nbr->htype != htype) {
			softirq_unlock();
			return -ENOENT;
		}
		else if (nbr->incomplete) {
			return -EINPROGRESS;
		}
		else if (nbr->hlen > hlen_max) {
			softirq_unlock();
			return -ENOMEM;
		}

		memcpy(out_haddr, &nbr->haddr[0], nbr->hlen);
	}
	softirq_unlock();

	return 0;
}

int neighbour_get_paddr(unsigned short htype, const void *haddr,
		const struct net_device *dev, unsigned short ptype,
		unsigned char plen_max, void *out_paddr) {
	struct neighbour *nbr;

	if ((haddr == NULL) || (dev == NULL) || (out_paddr == NULL)) {
		return -EINVAL;
	}

	softirq_lock();
	{
		nbr = nbr_lookup_by_haddr(htype, haddr, dev);
		if (nbr == NULL) {
			softirq_unlock();
			return -ENOENT;
		}
		else if (nbr->ptype != ptype) {
			softirq_unlock();
			return -ENOENT;
		}
		else if (nbr->plen > plen_max) {
			softirq_unlock();
			return -ENOMEM;
		}

		memcpy(out_paddr, &nbr->paddr[0], nbr->plen);
	}
	softirq_unlock();

	return 0;
}

int neighbour_del(unsigned short ptype, const void *paddr,
		const struct net_device *dev) {
	struct neighbour *nbr;

	if ((paddr == NULL) || (dev == NULL)) {
		return -EINVAL;
	}

	softirq_lock();
	{
		nbr = nbr_lookup_by_paddr(ptype, paddr, dev);
		if (nbr == NULL) {
			softirq_unlock();
			return -ENOENT;
		}

		nbr_free(nbr);
	}
	softirq_unlock();

	return 0;
}

int neighbour_clean(const struct net_device *dev) {
	struct neighbour *nbr;
	struct timeval now;

	ktime_get_timeval(&now);

	softirq_lock();
	{
		list_foreach(nbr, &neighbour_list, lnk) {
			NEIGHBOUR_CHECK_EXPIRE(nbr, &now);
			if ((nbr->dev == dev) || (dev == NULL)) {
				nbr_free(nbr);
			}
		}
	}
	softirq_unlock();

	return 0;
}

int neighbour_foreach(neighbour_foreach_ft func, void *args) {
	int ret;
	struct neighbour *nbr;
	struct timeval now;

	if (func == NULL) {
		return -EINVAL;
	}

	ktime_get_timeval(&now);

	softirq_lock();
	{
		list_foreach(nbr, &neighbour_list, lnk) {
			NEIGHBOUR_CHECK_EXPIRE(nbr, &now);
			ret = (*func)(nbr, args);
			if (ret != 0) {
				softirq_unlock();
				return ret;
			}
		}
	}
	softirq_unlock();

	return 0;
}
