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

#include <linux/interrupt.h>

#include <pnet/prior_path.h>
#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/pnet_pack.h>


EMBOX_UNIT_INIT(unit_init);

struct pack {
	struct list_head link;
	void *data;
};

OBJALLOC_DEF(common_pool, struct pack, CONFIG_PNET_RX_QUEUE_SIZE);

static LIST_HEAD(pnet_queue);

static void pnetif_rx_schedule(struct pack *pack) {
	list_add_tail(&pnet_queue, &pack->link);
	raise_softirq(PNET_RX_SOFTIRQ);
}

int netif_rx(void *data) {
	struct pack *pack;

	if (NULL == data) {
		return NET_RX_DROP;
	}

	pack = objalloc(&common_pool);
	INIT_LIST_HEAD(&pack->link);
	pack->data = data;

	pnetif_rx_schedule(pack);

	return NET_RX_SUCCESS;
}

static void pnet_rx_action(struct softirq_action *action) {
	struct pack *pack, *safe;
	uint32_t type;
	struct pnet_pack *skb_pack;
	net_node_t devs_resolver = pnet_get_module("devs resolver");

	list_for_each_entry_safe(pack, safe, &pnet_queue, link) {
		type = *(uint32_t*) pack->data;

		if ((type & 3)  == NET_TYPE) {
			skb_pack = pnet_pack_create(pack->data, 0, NET_TYPE);

			skb_pack->node = devs_resolver;

			pnet_entry(skb_pack);
		} else {
			pnet_entry((struct pnet_pack*) pack->data);
		}

		list_del(&pack->link);
		pool_free(&common_pool, pack);
	}
}

static int unit_init(void) {
	open_softirq(PNET_RX_SOFTIRQ, pnet_rx_action, NULL);

	return 0;
}
