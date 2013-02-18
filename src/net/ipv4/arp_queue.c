/**
 * @file
 * @brief Handle queue of packets waiting for ARP resolving.
 *
 * @date 13.12.11
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#include <string.h>
#include <net/arp_queue.h>
#include <util/hashtable.h>
#include <kernel/thread/event.h>
#include <embox/unit.h>
#include <sys/socket.h>
#include <errno.h>
#include <mem/misc/pool.h>
#include <net/arp.h>
#include <net/ip.h>
#include <kernel/time/time.h>
#include <kernel/time/ktime.h>
#include <framework/mod/options.h>
#include <assert.h>
#include <kernel/softirq_lock.h>
#include <net/route.h>
#include <err.h>


EMBOX_UNIT_INIT(arp_queue_init);

/**
 * Declaration module constants
 */
#define MODOPS_ARP_QUEUE_AMOUNT_PACKAGE  OPTION_GET(NUMBER, amount_package)
#define MODOPS_ARP_QUEUE_HTABLE_WIDTH    OPTION_GET(NUMBER, htable_width)
#define MODOPS_ARP_QUEUE_PACKAGE_TIMEOUT OPTION_GET(NUMBER, package_timeout)


/**
 * Packing for a sk_buff
 */
struct arp_queue_item {
	in_addr_t dest_ip;   /* IP address of destanation host */
	struct sk_buff *skb; /* outgoing package */
	uint32_t was_posted; /* time in which he was queued */
};


/**
 * Hashtable of awaiting packages
 * Structure:
 * 	 ip_addr => arp_queue_item
 * ip_addr -- destonation IP address
 * arp_queue_item -- waiting pakage
 */
static struct hashtable *arp_queue_table;

/* Allocators */
POOL_DEF(arp_queue_item_pool, struct arp_queue_item, MODOPS_ARP_QUEUE_AMOUNT_PACKAGE);

/* Get time in milliseconds */
static uint32_t get_msec(void) {
	return ktime_get_ns() / (NSEC_PER_USEC * USEC_PER_MSEC);
}

void arp_queue_process(struct sk_buff *arp_skb) {
	int ret;
	uint32_t now, lifetime;
	in_addr_t *resolved_ip;
	struct arp_queue_item *waiting_item;
#if 0
	struct event *sock_ready;
#endif
	struct arpg_stuff arph_stuff;

	assert(arp_skb != NULL);
	assert(arp_skb->nh.raw != NULL);

	now = get_msec();
	arpg_make_stuff(arp_skb->nh.arpgh, &arph_stuff);
	resolved_ip = (in_addr_t *) arph_stuff.spa;

	while ((waiting_item = hashtable_get(arp_queue_table, resolved_ip)) != NULL) {
		assert(waiting_item->skb != NULL);

		/* calculate experience */
		lifetime = now - waiting_item->was_posted;
		if (lifetime >= MODOPS_ARP_QUEUE_PACKAGE_TIMEOUT) {
			goto free_skb_and_item; /* time is up! drop this package */
		}

		/* try to rebuild */
		assert(waiting_item->skb->dev != NULL);
		assert(waiting_item->skb->dev->header_ops != NULL);
		assert(waiting_item->skb->dev->header_ops->rebuild != NULL);
		ret = waiting_item->skb->dev->header_ops->rebuild(waiting_item->skb);
		if (ret != 0) {
			LOG_ERROR("can't rebuild after resolving\n");
			goto free_skb_and_item; /* XXX it's not normal */
		}

#if 0
		/* save socket's event if it exist */
		sock_ready = ((waiting_item->skb->sk == NULL) ? NULL
				: &waiting_item->skb->sk->sock_is_ready);
#endif

		/* try to xmit */
		ret = dev_queue_xmit(waiting_item->skb);
		if (ret != 0) {
			LOG_ERROR("can't xmit over device\n");
#if 0
			/* notify owning socket */
			if (sock_ready != NULL) {
				event_notify(sock_ready);
			}
#endif
		}

		/* free resourse */
		goto free_item;

free_skb_and_item:
		skb_free(waiting_item->skb);
free_item:
		hashtable_del(arp_queue_table, resolved_ip);
		pool_free(&arp_queue_item_pool, waiting_item);
	}
}

int arp_queue_add(struct sk_buff *skb) {
	int ret;
	struct arp_queue_item *waiting_item;
	in_addr_t daddr;

	assert(skb != NULL);

	ret = rt_fib_route_ip(skb->nh.iph->daddr, &daddr);
	if (ret != 0) {
		return ret;
	}

	softirq_lock();

	waiting_item = (struct arp_queue_item *)pool_alloc(&arp_queue_item_pool);
	if (waiting_item == NULL) {
		return -ENOMEM;
	}

	waiting_item->was_posted = get_msec();
	waiting_item->skb = skb;
	waiting_item->dest_ip = daddr;

	ret = hashtable_put(arp_queue_table, (void *)&waiting_item->dest_ip, (void *)waiting_item);
	if (ret != 0) {
		pool_free(&arp_queue_item_pool, waiting_item);
		return ret;
	}

	softirq_unlock();

	ret = arp_send(ARP_OPER_REQUEST, ETH_P_ARP, skb->dev, daddr, skb->nh.iph->saddr, NULL,
			skb->dev->dev_addr, NULL);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

/* referencing to unaligned word pointer cannot be made on some arch,
 * ensuring it's aligned
 */
static in_addr_t addr_by_key(void *key) {
	const unsigned int mask = sizeof(in_addr_t) - 1;
	in_addr_t addr_buf = 0;

	if (((int) key & mask) == 0) {
		return * (in_addr_t *) key;
	}

	memcpy(&addr_buf, key, sizeof(in_addr_t));
	return addr_buf;
}

static size_t get_hash(void *key) {
	return (size_t) addr_by_key(key);
}

static int cmp_key(void *key1, void *key2) {
	in_addr_t ip1, ip2;

	ip1 = addr_by_key(key1);
	ip2 = addr_by_key(key2);
	return (ip1 < ip2 ? -1 : ip1 > ip2);
}

static int arp_queue_init(void) {
	arp_queue_table = hashtable_create(MODOPS_ARP_QUEUE_HTABLE_WIDTH, get_hash, cmp_key);
	if (arp_queue_table == NULL) {
		return -ENOMEM;
	}

	return ENOERR;
}
