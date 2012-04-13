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

OBJALLOC_DEF(common_pool, struct pack, OPTION_GET(NUMBER,rx_queue_size));

static LIST_HEAD(pnet_queue);

static void pnetif_rx_schedule(struct pack *pack) {
	list_add_tail(&pack->link,&pnet_queue);
	softirq_raise(PNET_RX_SOFTIRQ);
}

int netif_rx(void *data) {
	struct pack *pack;

	if (NULL == data) {
		return NET_RX_DROP;
	}

	if (!(pack = objalloc(&common_pool))) {
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&pack->link);
	pack->data = data;

	pnetif_rx_schedule(pack);
	return NET_RX_SUCCESS;
}

static void pnet_rx_action(softirq_nr_t nr, void *data) {
	struct pack *pack, *safe;
	uint32_t type;
	struct pnet_pack *skb_pack;
	net_node_t entry = pnet_get_module("pnet entry");

	list_for_each_entry_safe(pack, safe, &pnet_queue, link) {
		void *pack_data = pack->data;
		list_del(&pack->link);
		objfree(&common_pool, pack);
		type = *(uint32_t*) pack_data;

		if ((type & 3)  == NET_TYPE) {
			skb_pack = pnet_pack_create(pack_data, 0, NET_TYPE);

			skb_pack->node = entry;

			pnet_entry(skb_pack);
		} else {
			pnet_entry((struct pnet_pack*) pack_data);
		}

	}
}

static int unit_init(void) {
	softirq_install(PNET_RX_SOFTIRQ, pnet_rx_action, NULL);

	return 0;
}

PNET_NODE_DEF("pnet entry", {});

