/**
 * @file
 * @brief Implementation of ARP Cache
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

#include <framework/mod/options.h>
#define MODOPS_NEIGHBOUR_QUANTITY OPTION_GET(NUMBER, neighbour_quantity)
#define MODOPS_NEIGHBOUR_EXPIRE OPTION_GET(NUMBER, neighbour_expire)

struct neighbour_head {
	struct dlist_head lnk;
	time_t expire;
	struct neighbour n;
};

POOL_DEF(neighbour_pool, struct neighbour_head, MODOPS_NEIGHBOUR_QUANTITY); /* Pool of neighbour entities */
static DLIST_DEFINE(neighbour_list); /* List of valid entity */

#define NEIGHBOUR_CHECK_EXPIRE(neighbour_head, current_time)            \
	if ((neighbour_head->expire < current_time)                         \
			&& !(neighbour_head->n.flags & NEIGHBOUR_FLAG_PERMANENT)) { \
		dlist_del(&neighbour_head->lnk);                                \
		pool_free(&neighbour_pool, neighbour_head);                     \
		continue;                                                       \
	}

static struct neighbour_head * neighbour_get(const unsigned char *haddr,
		unsigned char hlen, const unsigned char *paddr, unsigned char plen,
		const struct net_device *dev, time_t current_time) {
	struct neighbour_head *nh, *tmp;

	assert(((haddr != NULL) && (hlen != 0)) || ((haddr == NULL) && (hlen == 0)));
	assert(((paddr != NULL) && (plen != 0)) || ((paddr == NULL) && (plen == 0)));
	assert((haddr != NULL) || (paddr != NULL));

	/* lookup entity with same hardware addr (if haddr not null),
	 * same protocol addr (if paddr not null) and the same
	 * device (if dev not null) */
	dlist_foreach_entry(nh, tmp, &neighbour_list, lnk) {
		NEIGHBOUR_CHECK_EXPIRE(nh, current_time);
		if (((haddr == NULL) || ((nh->n.hlen == hlen)
						&& (memcmp(nh->n.haddr, haddr, hlen) == 0)))
				&& ((paddr == NULL) || ((nh->n.plen == plen)
						&& (memcmp(nh->n.paddr, paddr, plen) == 0)))
				&& ((dev == NULL) || (nh->n.dev == dev))) {
			return nh;
		}
	}

	return NULL; /* not found */
}

int neighbour_add(const unsigned char *haddr, unsigned char hlen,
		const unsigned char *paddr, unsigned char plen,
		const struct net_device *dev, unsigned char flags) {
	struct neighbour_head *nh;

	assert((haddr != NULL) && (hlen != 0));
	assert((paddr != NULL) && (plen != 0));
	assert(dev != NULL);

	/* check arguments */
	if ((hlen > sizeof nh->n.haddr) || (plen > sizeof nh->n.paddr)) {
		return -EINVAL;
	}

	/* lock softirq context */
	softirq_lock();

	/* lookup existing entities */
	nh = neighbour_get(haddr, hlen, NULL, 0, NULL, 0);
	nh = ((nh != NULL) ? nh : neighbour_get(NULL, 0, paddr, plen, NULL, 0));
	if (nh != NULL) {
		/* free invalid entities */
		if (nh->n.flags & NEIGHBOUR_FLAG_PERMANENT) {
			softirq_unlock();
			return -EEXIST;
		}
		dlist_del(&nh->lnk);
	}
	else {
		/* allocate memory for new neighbour if required */
		nh = pool_alloc(&neighbour_pool);
		if (nh == NULL) {
			softirq_unlock();
			return -ENOMEM;
		}
		dlist_head_init(&nh->lnk);
	}

	/* initialize fields */
	nh->expire = time(NULL) + MODOPS_NEIGHBOUR_EXPIRE;
	nh->n.dev = dev;
	memcpy(&nh->n.haddr[0], haddr, hlen);
	nh->n.hlen = hlen;
	memcpy(&nh->n.paddr[0], paddr, plen);
	nh->n.plen = plen;
	nh->n.flags = flags;

	/* add to the tail */
	dlist_add_prev(&nh->lnk, &neighbour_list);

	/* unlock softirq context */
	softirq_unlock();

	return ENOERR;
}

int neighbour_del(const unsigned char *haddr, unsigned char hlen,
		const unsigned char *paddr, unsigned char plen,
		const struct net_device *dev) {
	struct neighbour_head *nh;

	assert(((haddr != NULL) && (hlen != 0)) || ((haddr == NULL) && (hlen == 0)));
	assert(((paddr != NULL) && (plen != 0)) || ((paddr == NULL) && (plen == 0)));
	assert((haddr != NULL) || (paddr != NULL));

	/* lock softirq context */
	softirq_lock();

	/* lookup entity */
	nh = neighbour_get(haddr, hlen, paddr, plen, dev, 0);
	if (nh == NULL) {
		softirq_unlock();
		return -ENOENT;
	}

	/* delete from list */
	dlist_del(&nh->lnk);
	pool_free(&neighbour_pool, nh);

	/* unlock softirq context */
	softirq_unlock();

	return ENOERR;
}

int neighbour_get_hardware_address(const unsigned char *paddr,
		unsigned char plen, const struct net_device *dev, unsigned char hlen_max,
		unsigned char *out_haddr, unsigned char *out_hlen) {
	struct neighbour_head *nh;

	assert((paddr != NULL) && (plen != 0));
	assert((hlen_max != 0) && (out_haddr != NULL));

	/* lock softirq context */
	softirq_lock();

	/* find an appropriate entry */
	nh = neighbour_get(NULL, 0, paddr, plen, dev, time(NULL));
	if (nh == NULL) {
		softirq_unlock();
		return -ENOENT;
	}

	/* check hardware address length */
	if (nh->n.hlen > hlen_max) {
		softirq_unlock();
		return -ENOMEM;
	}

	/* save results */
	memcpy(out_haddr, &nh->n.haddr[0], nh->n.hlen);
	if (out_hlen != NULL) {
		*out_hlen = nh->n.hlen;
	}
	else if (nh->n.hlen != hlen_max) {
		/* no such entity with the same length of hardware address */
		softirq_unlock();
		return -ENOENT;
	}

	/* unlock softirq context */
	softirq_unlock();

	return ENOERR;
}

int neighbour_get_protocol_address(const unsigned char *haddr,
		unsigned char hlen, const struct net_device *dev, unsigned char plen_max,
		unsigned char *out_paddr, unsigned char *out_plen) {
	struct neighbour_head *nh;

	assert((haddr != NULL) && (hlen != 0));
	assert((plen_max != 0) && (out_paddr != NULL));

	/* lock softirq context */
	softirq_lock();

	/* find an appropriate entry */
	nh = neighbour_get(haddr, hlen, NULL, 0, dev, time(NULL));
	if (nh == NULL) {
		softirq_unlock();
		return -ENOENT;
	}

	/* check protocol address length */
	if (nh->n.plen > plen_max) {
		softirq_unlock();
		return -ENOMEM;
	}

	/* save results */
	memcpy(out_paddr, &nh->n.paddr[0], nh->n.plen);
	if (out_plen != NULL) {
		*out_plen = nh->n.plen;
	}
	else if (nh->n.plen != plen_max) {
		/* no such entity with the same length of protocol address */
		softirq_unlock();
		return -ENOENT;
	}

	/* unlock softirq context */
	softirq_unlock();

	return ENOERR;
}

int neighbour_foreach(neighbour_foreach_handler_t func, void *args) {
	int ret;
	struct neighbour_head *nh, *tmp;
	time_t now;

	assert(func != NULL);

	/* get current time */
	time(&now);

	/* lock softirq context */
	softirq_lock();

	/* do for each entity */
	dlist_foreach_entry(nh, tmp, &neighbour_list, lnk) {
		NEIGHBOUR_CHECK_EXPIRE(nh, now);
		ret = (*func)(&nh->n, args);
		if (ret != ENOERR) {
			softirq_unlock();
			return ret;
		}
	}

	/* unlock softirq context */
	softirq_unlock();

	return ENOERR;
}
