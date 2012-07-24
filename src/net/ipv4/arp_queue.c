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
#include <util/list.h>
#include <assert.h>

EMBOX_UNIT_INIT(arp_queue_init);

/**
 * Declaration module constants
 */
#define ARP_QUEUE_AMOUNT_HOST     OPTION_GET(NUMBER, amount_host)
#define ARP_QUEUE_AMOUNT_PACKAGE  OPTION_GET(NUMBER, amount_package)
#define ARP_QUEUE_HTABLE_WIDTH    OPTION_GET(NUMBER, htable_width)
#define ARP_QUEUE_PACKAGE_TIMEOUT OPTION_GET(NUMBER, package_timeout)


/**
 * Packing for a sk_buff
 */
struct arp_queue_item {
	struct list_link lnk; /* for linking */
	struct sk_buff *skb;  /* outgoing package */
	uint32_t was_posted;  /* time in which he was queued */
};

/**
 * Wrapper for entity
 */
struct arp_queue {
	in_addr_t dest_ip; /* IP address of destanation host */
	struct list items; /* list of pending packages (struct arp_queue_item) */
};


/**
 * Hashtable of awaiting packages
 * Structure:
 * 	 ip_addr => slist
 * ip_addr -- destonation IP address
 * slist   -- queue a waiting pakages
 */
static struct hashtable *arp_queue_table;

/* Allocators */
POOL_DEF(arp_queue_pool, struct arp_queue, ARP_QUEUE_AMOUNT_HOST);
POOL_DEF(arp_queue_item_pool, struct arp_queue_item, ARP_QUEUE_AMOUNT_PACKAGE);

/* Get time in milliseconds */
static uint32_t get_msec(void) {
	return ktime_get_ns() / (NSEC_PER_USEC * USEC_PER_MSEC);
}

void arp_queue_process(struct sk_buff *arp_skb) {
	int res;
	uint32_t now, lifetime;
	in_addr_t resolved_ip;
	struct arp_queue *queue;
	struct arp_queue_item *waiting_item;
	struct event *sock_ready;

	assert(arp_skb != NULL);
	assert(arp_skb->nh.raw != NULL);

	now = get_msec();
	resolved_ip = arp_skb->nh.arph->ar_sip;

	queue = hashtable_get(arp_queue_table, &resolved_ip);
	if (queue == NULL) {
		return; /* no waiting packages for this host */
	}

	while (!list_is_empty(&queue->items)) {
		waiting_item = list_first_element(&queue->items, struct arp_queue_item, lnk);
		assert(waiting_item != NULL);
		assert(waiting_item->skb != NULL);
		/* calculate experience */
		lifetime = now - waiting_item->was_posted;
		if (lifetime >= ARP_QUEUE_PACKAGE_TIMEOUT) {
			skb_free(waiting_item->skb); /* time is up! drop this package */
		}
		else {
			/* try to rebuild */
			assert(waiting_item->skb->dev != NULL);
			assert(waiting_item->skb->dev->header_ops != NULL);
			assert(waiting_item->skb->dev->header_ops->rebuild != NULL);
			res = waiting_item->skb->dev->header_ops->rebuild(waiting_item->skb);
			if (res < 0) {
				continue; /* we still have time, let's try later */
			}
			/* save final event */
			assert(waiting_item->skb->sk != NULL);
			sock_ready = &waiting_item->skb->sk->sock_is_ready;
			/* try to xmit */
			if (dev_queue_xmit(waiting_item->skb) == ENOERR) {
				event_fire(sock_ready);
			}
		}
		/* free resourse */
		list_unlink_element(waiting_item, lnk);
		pool_free(&arp_queue_item_pool, waiting_item);
	}

	res = hashtable_del(arp_queue_table, &resolved_ip);
	assert(res == ENOERR);

	pool_free(&arp_queue_pool, queue);
}

int arp_queue_add(struct sk_buff *skb) {
	int res;
	in_addr_t dest_ip;
	struct arp_queue *queue;
	struct arp_queue_item *waiting_item;

	dest_ip = skb->nh.iph->daddr;

	/* get packages queue corresponding this destonation host */
	queue = hashtable_get(arp_queue_table, &dest_ip);
	if (queue == NULL) {
		/* create new queue if not this queue not found */
		queue = (struct arp_queue *)pool_alloc(&arp_queue_pool);
		if (queue == NULL) {
			return -ENOMEM;
		}
		/* initialize queue */
		queue->dest_ip = dest_ip;
		list_init(&queue->items);
		/* put to arp_queue_table */
		res = hashtable_put(arp_queue_table, &queue->dest_ip, queue);
		if (res < 0) {
			pool_free(&arp_queue_pool, queue);
			return res;
		}
	}

	/* allocate new node */
	waiting_item = (struct arp_queue_item *)pool_alloc(&arp_queue_item_pool);
	if (waiting_item == NULL) {
		return -ENOMEM; /* XXX We do not free `queue` even if it was just created */
	}

	waiting_item->was_posted = get_msec();
	waiting_item->skb = skb;
	list_link_init(&waiting_item->lnk);

	/* and push to queue */
	list_add_last_element(waiting_item, &queue->items, lnk);

	return ENOERR;
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
