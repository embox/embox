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
#include <pnet/prior_path.h>

#include <util/math.h>
#include <stdio.h>
#include <framework/mod/options.h>
#include <module/embox/pnet/rx_thread.h>

#define PNET_PRIORITY_COUNT \
	OPTION_MODULE_GET(embox__pnet__rx_thread, NUMBER, pnet_priority_count)

struct pnet_time_statistic {
	unsigned int total_usec;
	unsigned int avg_usec;
	unsigned int max_usec;
	unsigned short total_count; /**< Total count of packets with specified priority */
};

static struct pnet_time_statistic rx_time_stat[PNET_PRIORITY_COUNT]; /* microseconds */
static struct pnet_time_statistic tx_time_stat[PNET_PRIORITY_COUNT]; /* microseconds */

static int pnet_timer_hnd(struct pnet_pack *pack) {
	struct timespec ts;
	unsigned int elapsed_time;
	struct pnet_time_statistic *stat;

	if (pack->dir == PNET_PACK_DIRECTION_RX) {
		stat = &rx_time_stat[pack->priority];
	} else {
		stat = &tx_time_stat[pack->priority];
	}

	ktime_get_timespec(&ts);

	ts = timespec_sub(ts, pack->elapsed_time);
	printf("time: %d:%d (priority - %d)\n", (int)ts.tv_sec, (int)(ts.tv_nsec / 1000),
			pack->priority);

	elapsed_time = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;

	stat->total_count++;
	stat->max_usec = max(stat->max_usec, elapsed_time);
	stat->avg_usec = (stat->total_usec + elapsed_time) / stat->total_count;
	stat->total_usec += elapsed_time;

	return NET_HND_FORWARD_DEFAULT;
}

void pnet_print_times_per_priority(enum PNET_PACK_DIRECTION dir) {
	int i;
	struct pnet_time_statistic stat;

	printf("priority    total    avg    max    cnt of packets\n");
	for (i = 0; i < PNET_PRIORITY_COUNT; i++) {
		stat = dir == PNET_PACK_DIRECTION_RX ? rx_time_stat[i] : tx_time_stat[i];

		printf("%4d %12d %7d %7d %7d\n", i, stat.total_usec,
				stat.avg_usec, stat.max_usec, stat.total_count);
	}
}

PNET_NODE_DEF("timer", {
	.rx_hnd = pnet_timer_hnd,
	.tx_hnd = pnet_timer_hnd
});
