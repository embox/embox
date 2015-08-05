/**
 * @file
 *
 * @brief Single thread graph executer
 *
 * @date 28.10.2011
 * @author Anton Kozlov
 */

#include <pnet/core/core.h>
//#include <pnet/pack/pnet_pack.h>

int pnet_rx_thread_add(struct pnet_pack * pack) {
	pnet_process(pack);
	return 0;
}
