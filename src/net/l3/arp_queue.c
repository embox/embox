/**
 * @file
 * @brief Handle queue of packets waiting for ARP resolving.
 *
 * @date 13.12.11
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <embox/unit.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <kernel/printk.h>
#include <kernel/softirq_lock.h>
#include <kernel/time/ktime.h>
#include <mem/misc/pool.h>
#include <net/arp.h>
#include <net/arp_queue.h>
#include <net/if_arp.h>
#include <net/if_ether.h>
#include <net/inetdevice.h>
#include <net/route.h>
#include <string.h>
#include <sys/time.h>
#include <util/hashtable.h>

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
	struct timeval was_posted; /* time in which he was queued */
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

static int is_expired(struct timeval *since, useconds_t limit_msec) {
	struct timeval now, delta, limit;
	ktime_get_timeval(&now);
	timersub(&now, since, &delta);
	limit.tv_sec = limit_msec / MSEC_PER_SEC;
	limit.tv_usec = (limit_msec % MSEC_PER_SEC) * USEC_PER_MSEC;
	return timercmp(&delta, &limit, >=);
}

void arp_queue_process(const struct sk_buff *arp_skb) {
	in_addr_t *resolved_ip;
	struct arp_queue_item *waiting_item;
	struct arpg_stuff arph_stuff;

	assert(arp_skb != NULL);
	assert(arp_skb->nh.raw != NULL);

	arpg_make_stuff(arp_skb->nh.arpgh, &arph_stuff);
	resolved_ip = (in_addr_t *) arph_stuff.spa;

	while ((waiting_item = hashtable_get(arp_queue_table, resolved_ip)) != NULL) {
		assert(waiting_item->skb != NULL);

		if (!is_expired(&waiting_item->was_posted,
					MODOPS_ARP_QUEUE_PACKAGE_TIMEOUT)) {
			/* try to rebuild */
			assert(waiting_item->skb->dev != NULL);
			assert(waiting_item->skb->dev->header_ops != NULL);
			assert(waiting_item->skb->dev->header_ops->rebuild != NULL);
			if (0 == waiting_item->skb->dev->header_ops->rebuild(
						waiting_item->skb)) {
				/* try to xmit */
				if (0 != dev_xmit_skb(waiting_item->skb)) {
					printk("arp_queue_process: erorr: can't xmit over device\n");
				}
			}
			else {
				printk("arp_queue_process: error: can't rebuild after resolving\n");
				skb_free(waiting_item->skb);
			}
		}
		else {
			skb_free(waiting_item->skb);
		}

		hashtable_del(arp_queue_table, resolved_ip);
		pool_free(&arp_queue_item_pool, waiting_item);
	}
}

int arp_queue_wait_resolve(struct sk_buff *skb) {
	int ret;
	struct arp_queue_item *waiting_item;
	in_addr_t saddr, daddr;
	struct in_device *in_dev;

	assert(skb != NULL);
	assert(skb->dev != NULL);

	in_dev = inetdev_get_by_dev(skb->dev);
	assert(in_dev != NULL);

	saddr = in_dev->ifa_address;

	ret = rt_fib_route_ip(skb->nh.iph->daddr, &daddr);
	if (ret != 0) {
		return ret;
	}

	softirq_lock();
	{
		waiting_item = (struct arp_queue_item *)pool_alloc(&arp_queue_item_pool);
		if (waiting_item == NULL) {
			softirq_unlock();
			return -ENOMEM;
		}
	}
	softirq_unlock();

	ktime_get_timeval(&waiting_item->was_posted);
	waiting_item->skb = skb;
	waiting_item->dest_ip = daddr;

	softirq_lock();
	{
		ret = hashtable_put(arp_queue_table, (void *)&waiting_item->dest_ip, (void *)waiting_item);
		if (ret != 0) {
			pool_free(&arp_queue_item_pool, waiting_item);
			softirq_unlock();
			return ret;
		}
	}
	softirq_unlock();

	ret = arp_send(ARP_OPER_REQUEST, ETH_P_IP, skb->dev->addr_len,
			sizeof saddr, NULL, &saddr, NULL, &daddr, NULL, skb->dev);
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
	return ip1 < ip2 ? -1 : ip1 > ip2;
}

static int arp_queue_init(void) {
	arp_queue_table = hashtable_create(MODOPS_ARP_QUEUE_HTABLE_WIDTH,
			get_hash, cmp_key);
	if (arp_queue_table == NULL) {
		return -ENOMEM;
	}

	return 0;
}
