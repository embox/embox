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

EMBOX_UNIT_INIT(arp_queue_init);

#define ARP_QUEUE_SIZE      OPTION_GET(NUMBER,amount_entity)
#define ARP_HASHTABLE_WIDTH OPTION_GET(NUMBER,htable_width)
#define ARP_QUEUE_TIMEOUT   OPTION_GET(NUMBER,packet_timeout)

struct htable_entity {
	in_addr_t dest_ip;
	struct sk_buff *skb;
	uint32_t posted;
};

static struct hashtable *arp_wait_table;
POOL_DEF(htable_entity_pool, struct htable_entity, ARP_QUEUE_SIZE);

uint32_t get_msec(void) {
	struct timeval tv;
	ktime_get_timeval(&tv);
	return (uint32_t)tv.tv_sec * MSEC_PER_SEC + (uint32_t)tv.tv_usec / USEC_PER_MSEC;
}

void arp_queue_process(struct sk_buff *arp_pack) {
	struct htable_entity *entity;
	in_addr_t dst;
	uint32_t now;
	struct sk_buff *outgoing_skb;

	dst = arp_pack->nh.arph->ar_sip;
	now = get_msec();
	while ((entity = hashtable_get(arp_wait_table, (void *)&dst)) != NULL) {
		outgoing_skb = entity->skb;
		if (now - entity->posted < ARP_QUEUE_TIMEOUT) {
			/* rebuild packet */
			if (outgoing_skb->dev->header_ops->rebuild(outgoing_skb) < 0) {
				skb_free(outgoing_skb);
			}
			else {
				/* xmit skb if ok */
				if (dev_queue_xmit(outgoing_skb) == ENOERR) {
					event_fire(&outgoing_skb->sk->sock_is_ready); /* if ok */
				}
			}
		}
		else {
			skb_free(outgoing_skb); // timeout
		}
		/* XXX there is we want delete `entity` from hashtable */
		hashtable_del(arp_wait_table, (void *)&dst);
		pool_free(&htable_entity_pool, entity);
	}
}

int arp_queue_add(struct sk_buff *skb) {
	int res;
	struct htable_entity *entity;

	entity = (struct htable_entity *)pool_alloc(&htable_entity_pool);
	if (entity == NULL) {
		return -ENOMEM;
	}
	entity->dest_ip = skb->nh.iph->daddr;
	entity->posted = get_msec();
	entity->skb = skb;

	res = hashtable_put(arp_wait_table, (void *)&entity->dest_ip, (void *)entity);
	if (res < 0) {
		pool_free(&htable_entity_pool, entity);
		return res;
	}

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
	arp_wait_table = hashtable_create(ARP_HASHTABLE_WIDTH, get_hash, cmp_key);
	if (arp_wait_table == NULL) {
		return -ENOMEM;
	}

	return ENOERR;
}
