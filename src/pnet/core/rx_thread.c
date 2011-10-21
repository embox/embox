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

int rx_thread_add(net_packet_t pack) {
	net_prior_t prior = pack->node->prior;

	c_buf_add(&c_bufs[prior], pack);

	thread_resume(pnet_rx_threads[prior]);

	return 0;
}

#if 0
#include <net/in.h>
#include <pnet/prior_path.h>
#include <kernel/thread/api.h>
#include <net/skbuff.h>

#if 0
int netif_rx(struct sk_buff skb) {
	netif_add_rx_pack(skb);
	softirq_fire();

	return 0;
}
#endif
//TODO move from here
/** get next received packet from common rx queue */
static struct sk_buff *netif_get_rx_pack(void) {
	return NULL;
}

int netif_rx_dispatch(void) {
//	struct pnet_path *prior_path;
	struct sk_buff *skb;

	while(NULL != (skb = netif_get_rx_pack())) {
//		prior_path = pnet_get_dev_prior(skb->dev);

	}

	return 0;
}
#endif
