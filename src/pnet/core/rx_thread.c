/**
 * @file
 * @brief Multi-thread (multi-priority) graph executer
 *
 * @date 19.10.2011
 * @author Anton Bondarev
 * @author Anton Kozlov
 */

#include <embox/unit.h>
#include <lib/libds/ring_buff.h>
#include <stdio.h>

#include <kernel/thread.h>
#include <util/err.h>
#include <kernel/event.h>

#include <pnet/core/core.h>
#include <pnet/pack/pnet_pack.h>

//#define PNET_THREAD_DEBUG

EMBOX_UNIT_INIT(rx_thread_init);

#define RX_THRD_BUF_SIZE   0x10

#define PNET_PRIORITY_COUNT  OPTION_GET(NUMBER, pnet_priority_count)

static struct thread *pnet_rx_threads[PNET_PRIORITY_COUNT];

#ifdef PNET_THREAD_DEBUG
static int running[PNET_PRIORITY_COUNT];
static int sleeping[PNET_PRIORITY_COUNT];
#endif

struct pnet_wait_unit {
	struct event event;
	struct ring_buff buff;
};

static net_packet_t pack_bufs[PNET_PRIORITY_COUNT][RX_THRD_BUF_SIZE];
static struct pnet_wait_unit pack_storage[PNET_PRIORITY_COUNT];

static void *pnet_rx_thread_hnd(void *args) {
	struct pnet_wait_unit *unit = (struct pnet_wait_unit *) args;
	struct pnet_pack *pack;

	while (1) {
		if (unit->buff.cnt == 0) {
			EVENT_WAIT(&unit->event, unit->buff.cnt, EVENT_TIMEOUT_INFINITE);
			continue;
		}
		ring_buff_dequeue(&unit->buff, &pack, 1);
		pnet_process(pack);
	}
	return NULL;
}

static int rx_thread_init(void) {
	for (size_t i = 0; i < PNET_PRIORITY_COUNT; i++) {

		event_init(&pack_storage[i].event, "pack_arrived");

		ring_buff_init(&pack_storage[i].buff, sizeof(net_packet_t), RX_THRD_BUF_SIZE,
				(void *) pack_bufs[i]);
		pnet_rx_threads[i] = thread_create(0, pnet_rx_thread_hnd, &pack_storage[i]);
		if(ptr2err(pnet_rx_threads[i])) {
			return -1;
		}

		schedee_priority_set(&pnet_rx_threads[i]->schedee, SCHED_PRIORITY_NORMAL + 1 + i);
	}

	return 0;
}

int pnet_rx_thread_add(struct pnet_pack *pack) {
	uint32_t prio;

	prio = pack->priority;

	if (pack->stat.last_sync != (clock_t)-1) {
		/* We are not in differed irq handler */
		if ((thread_self() != pnet_rx_threads[prio])) {
			/* If we will switched to thread with higher priority, than calculate running time in current thread
			 * and initialize start timestamp in new thread */
			pack->stat.running_time += thread_get_running_time(thread_self()) - pack->stat.last_sync;
			pack->stat.last_sync = thread_get_running_time(pnet_rx_threads[prio]);
		}
	} else {
		/* We are in differed irq handler */
		pack->stat.last_sync = thread_get_running_time(pnet_rx_threads[prio]);
	}

	ring_buff_enqueue(&pack_storage[prio].buff, &pack, 1);
	event_notify(&pack_storage[prio].event);

	return 0;
}
