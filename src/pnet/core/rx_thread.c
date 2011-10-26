/**
 * @file
 *
 * @brief
 *
 * @date 19.10.2011
 * @author Anton Bondarev
 * @author Anton Kozlov
 */

#include <pnet/core.h>
#include <util/circular_buffer.h>
#include <kernel/thread/api.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(rx_thread_init);

#define RX_THRD_CNT PNET_PRIORITY_COUNT
#define RX_THRD_BUF_SIZE 0x10

static struct thread *pnet_rx_threads[RX_THRD_CNT];

static void *pnet_rx_thread_hnd(void *args) {
	struct c_buf *buf = (struct c_buf *) args;
	net_packet_t pack;
	while (1) {
		if (buf->size == 0) {
			thread_suspend(thread_self());
			continue;
		}
		c_buf_get(buf, pack);
		pnet_process(pack);
	}
	return NULL;
}

static net_packet_t pack_bufs[RX_THRD_CNT][RX_THRD_BUF_SIZE];
static struct c_buf c_bufs[RX_THRD_CNT];

static int rx_thread_init(void) {
	for (int i = 0; i < RX_THRD_CNT; i++) {
		c_buf_init(&c_bufs[i], RX_THRD_BUF_SIZE, (void *) pack_bufs[i]);
		thread_create(&pnet_rx_threads[i], 0, pnet_rx_thread_hnd, &c_bufs[i]);
	}
	return 0;
}

int pnet_rx_thread_add(net_packet_t pack) {
#if 0
	net_prior_t prior = pack->node->prior;

	c_buf_add(&c_bufs[prior], pack);

	thread_resume(pnet_rx_threads[prior]);
#endif
	pnet_process(pack);
	return 0;
}

