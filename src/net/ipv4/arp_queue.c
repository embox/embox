/**
 * @file
 * @brief Handle queue of packets waiting for ARP resolving.
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

EMBOX_UNIT_INIT(arp_queue_init);

#define ARP_QUEUE_SIZE      0x10
#define ARP_HASHTABLE_WIDTH 10

struct htable_entity {
	in_addr_t dest_ip;
	struct event resolved;
	char reply_received;
};

static struct hashtable *arp_wait_table;
POOL_DEF(htable_entity_pool, struct htable_entity, ARP_QUEUE_SIZE);

void arp_queue_process(struct sk_buff *arp_pack) {
	struct htable_entity *entity;
	in_addr_t dst;

	dst = arp_pack->nh.arph->ar_sip;
	entity = hashtable_get(arp_wait_table, (void *)&dst);
	if (entity != NULL) {
		event_fire(&entity->resolved);
		entity->reply_received = 1;
		// TODO
//		hashtable_del(arp_wait_table, (void *)&dst);
//		pool_free(&htable_entity_pool, entity);
	}
}

int arp_queue_add(struct sk_buff *skb) {
	struct htable_entity *entity;
	in_addr_t dst;

	dst = skb->nh.iph->daddr;
	entity = hashtable_get(arp_wait_table, (void *)&dst);
	if (entity != NULL) {
		return ENOERR;
	}

	entity = (struct htable_entity *)pool_alloc(&htable_entity_pool);
	if (entity == NULL) {
		return -ENOMEM;
	}
	entity->dest_ip = dst;
	event_init(&entity->resolved, NULL);
	entity->reply_received = 0;

	hashtable_put(arp_wait_table, (void *)&entity->dest_ip, (void *)entity);

	return ENOERR;
}

int arp_queue_wait_resolve(struct sk_buff *skb) {
	struct htable_entity *entity;
	in_addr_t dst;

	dst = skb->nh.iph->daddr;
	entity = hashtable_get(arp_wait_table, (void *)&dst);
	if (entity == NULL) {
		return -EINVAL;
	}

	sched_sleep(&entity->resolved, MAX_WAIT_TIME);

	return (entity->reply_received ? ENOERR : -ENOENT);
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
