/**
 * @file
 * @brief  Entry net for pnet version network subsystem.
 *
 * @date 27.10.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <embox/unit.h>
#include <mem/objalloc.h>
#include <linux/list.h>
#include <stdio.h>

#include <pnet/prior_path.h>
#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/pnet_pack.h>

#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>

#define PNET_RX_HND_PRIORITY OPTION_GET(NUMBER, hnd_priority)

EMBOX_UNIT_INIT(unit_init);

static LIST_HEAD(skb_queue);
static LIST_HEAD(pnet_queue);

static struct lthread pnet_rx_handler_lt;

int netif_rx(void *data) {
	struct pnet_pack *pack;
	uint32_t type;

	if (NULL == data) {
		return NET_RX_DROP;
	}

	type = *(uint32_t*) data;

	if ((type & 3)  == PNET_PACK_TYPE_SKB) {
		INIT_LIST_HEAD((struct list_head *) data);
		list_add_tail((struct list_head *) data, &skb_queue);
	} else {
		pack = (struct pnet_pack*) data;
		INIT_LIST_HEAD(&pack->link);
		list_add_tail(&pack->link, &pnet_queue);
	}

	lthread_launch(&pnet_rx_handler_lt);

	return NET_RX_SUCCESS;
}

static net_node_t entry;

static int pnet_rx_action(struct lthread *data) {
	struct pnet_pack *pack, *safe;
	struct list_head *curr, *n;
	struct pnet_pack *skb_pack;

	list_for_each_entry_safe(pack, safe, &pnet_queue, link) {
		list_del(&pack->link);
		pnet_entry(pack);
	}

	list_for_each_safe(curr, n, &skb_queue) {
		list_del(curr);
		skb_pack = pnet_pack_create((void*) curr, 0, PNET_PACK_TYPE_SKB);
		skb_pack->node = entry;
		pnet_entry(skb_pack);
	}

	return NULL;
}

static int unit_init(void) {
	entry = pnet_get_module("pnet entry");

	lthread_init(&pnet_rx_handler_lt, &pnet_rx_action);
	schedee_priority_set(&pnet_rx_handler_lt.schedee, PNET_RX_HND_PRIORITY);

	return 0;
}

PNET_NODE_DEF("pnet entry", {});
