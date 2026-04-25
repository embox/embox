/**
 * @file
 *
 * @date 23.04.2012
 * @author Anton Bondarev
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>

#include <net/netdevice.h>
#include <net/can_netdevice.h>

#include <net/sock.h>

#include <hal/ipl.h>
#include <net/netdevice.h>
#include <net/skbuff.h>

#include <kernel/sched/schedee_priority.h>
#include <kernel/lthread/lthread.h>


#include <lib/libds/dlist.h>

#define CANIF_RX_HND_PRIORITY 200

static DLIST_DEFINE(canif_rx_list);

static int canif_rx_action(struct lthread *self);
static LTHREAD_DEF(canif_rx_irq_handler, canif_rx_action, CANIF_RX_HND_PRIORITY);

#if 0
static int canif_tx_action(struct lthread *self);
static LTHREAD_DEF(canif_tx_handler, canif_tx_action, CANIF_RX_HND_PRIORITY);
#endif

extern const struct sock_proto_ops *const af_can_sock_ops;

static int can_rcv_tester(const struct sock *sk,
						const struct sk_buff *skb) {
	assert(sk != NULL);
	return (sk->opt.so_domain == AF_CAN);
}

void cansock_rx(struct sk_buff *skb) {
	struct sock *sk;
	sk = sock_lookup(NULL, af_can_sock_ops, can_rcv_tester, skb);
	if (sk != NULL) {
			sock_rcv(sk, skb, skb->mac.raw, skb->len);
		}
}

static int canif_rx_action(struct lthread *self) {
	struct net_device *dev = NULL;
	ipl_t ipl;

	ipl= ipl_save();
	{
		dlist_foreach_entry_safe(dev, &canif_rx_list, rx_lnk) {
			struct sk_buff *skb;

			while ((skb = skb_queue_pop(&dev->dev_queue)) != NULL) {
				ipl_restore(ipl);
				{
					cansock_rx(skb);
				}
				ipl= ipl_save();
			}
			dlist_del_init(&dev->rx_lnk);
		}
	}
	ipl_restore(ipl);

	return 0;
}


/* we can be in irq mode */
int canif_rx(void *data) {
	ipl_t ipl;
	struct sk_buff *skb = data;
	struct net_device *dev;

	assert(skb != NULL);
	assert(skb->dev != NULL);

	dev = skb->dev;

	ipl = ipl_save();
	{
		skb_queue_push(&dev->dev_queue, skb);

		if (dlist_empty(&dev->rx_lnk)) {
			dlist_add_prev(&dev->rx_lnk, &canif_rx_list);
		}

		lthread_launch(&canif_rx_irq_handler);
	}
	ipl_restore(ipl);

	return 0;
}