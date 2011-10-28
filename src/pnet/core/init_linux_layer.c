/**
 * @file
 * @brief Network core
 *
 * @date 20.10.2011
 * @author Anton Kozlov
 */

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/match.h>

#include <mem/objalloc.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(net_core_init);

static int net_core_init(void) {
	net_node_t devs = pnet_dev_get_entry();
	net_node_t gate = pnet_get_node_linux_gate();

	pnet_node_attach(devs, NET_RX_DFAULT, gate);

	return 0;
}
