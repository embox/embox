/**
 * @file
 * @brief
 *
 * @date 02.11.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <assert.h>
#include <pnet/prior_path.h>

#include <pnet/core.h>
#include <pnet/node.h>

int pnet_entry(net_packet_t pack) {
	if (!(pack && pack->node && pack->node->rx_dfault)) {
		return -EINVAL;
	}
	pack->node = pack->node->rx_dfault;
	return pnet_rx_thread_add(pack);
}


