/**
 * @file
 *
 * @brief
 *
 * @date 28.10.2011
 * @author Anton Kozlov
 */

#include <pnet/core.h>

int pnet_rx_thread_add(net_packet_t pack) {
	pnet_process(pack);
	return 0;
}


