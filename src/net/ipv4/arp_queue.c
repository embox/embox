/**
 * @file
 * @brief Handle queue of packets waiting for ARP resolving.
 *
 * @date 13.12.11
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#include <net/arp_queue.h>
#include <util/hashtable.h>
#include <kernel/thread/event.h>
#include <embox/unit.h>
#include <net/socket.h>
#include <errno.h>
#include <mem/misc/pool.h>
#include <net/arp.h>
#include <net/ip.h>
#include <kernel/time/time_types.h>
#include <kernel/time/ktime.h>
#include <framework/mod/options.h>
#include <assert.h>
#include <kernel/softirq_lock.h>
#include <net/route.h>

EMBOX_UNIT_INIT(arp_queue_init);

/**
 * Declaration module constants
 */
#define ARP_QUEUE_AMOUNT_PACKAGE  OPTION_GET(NUMBER, amount_package)
#define ARP_QUEUE_HTABLE_WIDTH    OPTION_GET(NUMBER, htable_width)
#define ARP_QUEUE_PACKAGE_TIMEOUT OPTION_GET(NUMBER, package_timeout)


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
POOL_DEF(arp_queue_item_pool, struct arp_queue_item, ARP_QUEUE_AMOUNT_PACKAGE);

/* Get time in milliseconds */
static uint32_t get_msec(void) {
	return ktime_get_ns() / (NSEC_PER_USEC * USEC_PER_MSEC);
}

void arp_queue_process(struct sk_buff *arp_skb) {
	int res;
	uint32_t now, lifetime;
	in_addr_t resolved_ip;
	struct arp_queue_item *waiting_item;
	struct event *sock_ready;

	assert(arp_skb != NULL);
	assert(arp_skb->nh.raw != NULL);

	now = get_msec();
	resolved_ip = arp_skb->nh.arph->ar_sip;

	while ((waiting_item = hashtable_get(arp_queue_table, &resolved_ip)) != NULL) {
		assert(waiting_item->skb != NULL);

		/* calculate experience */
		lifetime = now - waiting_item->was_posted;
		if (lifetime >= ARP_QUEUE_PACKAGE_TIMEOUT) {
			goto free_skb_and_item; /* time is up! drop this package */
		}

		/* try to rebuild */
		assert(waiting_item->skb->dev != NULL);
		assert(waiting_item->skb->dev->header_ops != NULL);
		assert(waiting_item->skb->dev->header_ops->rebuild != NULL);
		res = waiting_item->skb->dev->header_ops->rebuild(waiting_item->skb);
		if (res < 0) {
			goto free_skb_and_item; /* XXX it's not normal */
		}

		/* save socket's event if it exist */
		sock_ready = ((waiting_item->skb->sk == NULL) ? NULL
				: &waiting_item->skb->sk->sock_is_ready);

		/* try to xmit */
		if (dev_queue_xmit(waiting_item->skb) == ENOERR) {
			/* notify owning socket */
			if (sock_ready != NULL) {
				event_notify(sock_ready);
			}
		}

		/* free resourse */
		goto free_item;

free_skb_and_item:
		skb_free(waiting_item->skb);
free_item:
		hashtable_del(arp_queue_table, &resolved_ip);
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

	ret = arp_send(ARPOP_REQUEST, ETH_P_ARP, skb->dev, daddr, skb->nh.iph->saddr, NULL,
			skb->dev->dev_addr, NULL);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

static size_t get_hash(void *key) {
	return (size_t)*(in_addr_t *)key;
}

static int cmp_key(void *key1, void *key2) {
	in_addr_t ip1, ip2;

	ip1 = *(in_addr_t *)key1;
	ip2 = *(in_addr_t *)key2;
	return (ip1 < ip2 ? -1 : ip1 > ip2);
}

static int arp_queue_init(void) {
	arp_queue_table = hashtable_create(ARP_QUEUE_HTABLE_WIDTH, get_hash, cmp_key);
	if (arp_queue_table == NULL) {
		return -ENOMEM;
	}

	return ENOERR;
}
