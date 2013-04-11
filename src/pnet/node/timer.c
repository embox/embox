/**
 * @file
 *
 * @brief
 *
 * @date 12.04.2013
 * @author Alexander Kalmuk
 */

#include <pnet/core.h>
#include <pnet/repo.h>
#include <pnet/pnet_pack.h>
#include <stdio.h>
#include <pnet/prior_path.h>
#include <framework/mod/options.h>
#include <module/embox/pnet/rx_thread.h>

#define PNET_PRIORITY_COUNT \
	OPTION_MODULE_GET(embox__pnet__rx_thread, NUMBER, pnet_priority_count)

static unsigned int times[PNET_PRIORITY_COUNT]; /* microseconds */

static int pnet_timer_rx(struct pnet_pack *pack) {
	struct timespec ts;

	ktime_get_timespec(&ts);

	ts = timespec_sub(ts, pack->elapsed_time);
	printf("time: %d:%d (priority - %d)\n", (int)ts.tv_sec, (int)(ts.tv_nsec / 1000),
			pack->priority);

	times[pack->priority] += ts.tv_sec * 1000000 + ts.tv_nsec / 1000;

	return NET_HND_FORWARD_DEFAULT;
}

PNET_NODE_DEF("timer", {
	.rx_hnd = pnet_timer_rx,
	.tx_hnd = NULL
});

