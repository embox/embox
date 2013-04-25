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
#include <profiler/tracing/trace.h>

#include <pnet/core.h>
#include <pnet/node.h>

int pnet_entry(struct pnet_pack *pack) {
	struct __trace_point *p;

	if (!(pack && pack->node && pack->node->rx_dfault)) {
		return -EINVAL;
	}
	pack->node = pack->node->rx_dfault;

	pack->stat.start_time = clock();

	pack->stat.last_sync = -1;

	p = trace_point_get_by_name("interrupt");
	pack->stat.interrupt_count = trace_point_get_value(p);

	pnet_rx_thread_add(pack);
	return 0;
}
