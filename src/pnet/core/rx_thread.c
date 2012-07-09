/**
 * @file
 * @brief Multi-thread (multi-priority) graph executer
 *
 * @date 19.10.2011
 * @author Anton Bondarev
 * @author Anton Kozlov
 */


#include <embox/unit.h>
#include <util/ring_buff.h>
#include <stdio.h>

#include <kernel/thread/api.h>
#include <kernel/thread/state.h>

#include <pnet/core.h>
#include <pnet/pnet_pack.h>

//#define PNET_THREAD_DEBUG

EMBOX_UNIT_INIT(rx_thread_init);

#define RX_THRD_BUF_SIZE   0x10

static struct thread *pnet_rx_threads[CONFIG_PNET_PRIORITY_COUNT];

#ifdef PNET_THREAD_DEBUG
static int running[CONFIG_PNET_PRIORITY_COUNT];
static int suspended[CONFIG_PNET_PRIORITY_COUNT];
#endif

static void *pnet_rx_thread_hnd(void *args) {
	struct ring_buff *buf = (struct ring_buff *) args;
	struct pnet_pack *pack;

	while (1) {
		if (buf->cnt == 0) {
			thread_suspend(thread_self());
			continue;
		}
		ring_buff_deque(buf, &pack);
		pnet_process(pack);
	}
	return NULL;
}

static net_packet_t pack_bufs[CONFIG_PNET_PRIORITY_COUNT][RX_THRD_BUF_SIZE];
static struct ring_buff c_bufs[CONFIG_PNET_PRIORITY_COUNT];

static int rx_thread_init(void) {
	for (size_t i = 0; i < CONFIG_PNET_PRIORITY_COUNT; i++) {
		ring_buff_init(&c_bufs[i], sizeof(net_packet_t), RX_THRD_BUF_SIZE,
				(void *) pack_bufs[i]);
		thread_create(&pnet_rx_threads[i], 0, pnet_rx_thread_hnd, &c_bufs[i]);

		thread_set_priority(pnet_rx_threads[i], THREAD_PRIORITY_DEFAULT + 1 + i);

#ifdef PNET_THREAD_DEBUG
		for (int i = 0; i < CONFIG_PNET_PRIORITY_COUNT; i++) {
			running[i] = suspended[i] = 0;
		}
#endif
	}

	return 0;
}

int pnet_rx_thread_add(struct pnet_pack *pack) {
	uint32_t prio;

	prio = pack->priority;
	ring_buff_enque(&c_bufs[prio], &pack);
	thread_resume(pnet_rx_threads[prio]);

#ifdef PNET_THREAD_DEBUG
	if (thread_state_running(pnet_rx_threads[prio]->state)) {
		running[prio]++;
	}

	if (thread_state_suspended(pnet_rx_threads[prio]->state)) {
		suspended[prio]++;
	}
	printf("%d : run %d times, suspended %d times\n", pnet_rx_threads[prio]->id,
				running[prio], suspended[prio]);
#endif

	return 0;
}
