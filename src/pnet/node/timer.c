/**
 * @file
 *
 * @brief
 *
 * @date 12.04.2013
 * @author Alexander Kalmuk
 */

#include <fcntl.h>
#include <stdio.h>

#include <util/math.h>

#include <kernel/thread.h>

#include <pnet/core.h>
#include <pnet/repo.h>
#include <pnet/pnet_pack.h>
#include <pnet/prior_path.h>

#include <framework/mod/options.h>
#include <module/embox/pnet/rx_thread.h>
#include <embox/unit.h>

EMBOX_UNIT(init, fini);

#define PNET_PRIORITY_COUNT \
	OPTION_MODULE_GET(embox__pnet__rx_thread, NUMBER, pnet_priority_count)

#define ITERATION_CNT 100

static FILE *results;

static int pnet_timer_hnd(struct pnet_pack *pack) {
	struct timespec ts;
	unsigned int elapsed_time;

	static int k;

	ktime_get_timespec(&ts);

	//elapsed_time = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
	elapsed_time = clock() - pack->stat.start_time;

	/* total time */
	if (pack->priority == 2) {
		if (++k <= ITERATION_CNT) {
			fprintf(results, "pnet_total: %d\n", elapsed_time);
		} else {
			printf("K:%d\n", k);
		}
	}

	return NET_HND_FORWARD_DEFAULT;
}

PNET_NODE_DEF("timer", {
	.rx_hnd = pnet_timer_hnd,
	.tx_hnd = pnet_timer_hnd
});

static int init(void) {
	results = fopen("/tmp/pnet_total", "rw");
	return (NULL == results);
}

static int fini(void) {
	return fclose(results);
}
