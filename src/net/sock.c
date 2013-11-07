/**
 * @file
 * @brief Generic socket support routines.
 *
 * @date 12.08.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <embox/net/family.h>
#include <embox/net/pack.h>
#include <embox/net/sock.h>
#include <errno.h>
#include <hal/ipl.h>
#include <kernel/task/io_sync.h>
#include <mem/misc/pool.h>
#include <net/sock.h>
#include <net/skbuff.h>
#include <string.h>
#include <util/math.h>

void sock_rcv(struct sock *sk, struct sk_buff *skb,
		unsigned char *p_data, size_t size) {
	if ((sk == NULL) || (skb == NULL) || (p_data == NULL)) {
		return; /* error: invalid argument */
	}

	if (sk->shutdown_flag & (SHUT_RD + 1)) {
		skb_free(skb);
		return; /* error: socket is down */
	}

	skb->sk = sk;
	skb->p_data = p_data;
	skb->p_data_end = p_data + size;

	skb_queue_push(&sk->rx_queue, skb);

	io_sync_enable(&sk->ios, IO_SYNC_READING);
}

int sock_close(struct sock *sk) {
	if (sk == NULL) {
		return -EINVAL;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->close == NULL) {
		sock_release(sk);
		return 0;
	}

	return sk->f_ops->close(sk);
}

int sock_common_recvmsg(struct sock *sk, struct msghdr *msg,
		int flags, int stream_mode) {
	struct sk_buff *skb;
	char *buff;
	size_t buff_sz, total_len, len;

	if ((sk == NULL) || (msg == NULL)) {
		return -EINVAL;
	}

	assert(msg->msg_iov != NULL);
	buff = msg->msg_iov->iov_base;
	buff_sz = msg->msg_iov->iov_len;
	assert((buff != NULL) || (buff_sz == 0));
	total_len = 0;

	while (1) {
		io_sync_disable(&sk->ios, IO_SYNC_READING);
		skb = skb_queue_front(&sk->rx_queue);
		if (skb == NULL) {
			if (total_len == 0) {
				return -EAGAIN;
			}
			break;
		}

		len = min(buff_sz, skb->p_data_end - skb->p_data);

		memcpy(buff, skb->p_data, len);
		buff += len;
		buff_sz -= len;
		skb->p_data += len;
		total_len += len;

		if (!stream_mode || (skb->p_data >= skb->p_data_end)) {
			skb_free(skb);
		}

		if (!stream_mode || (buff_sz == 0)) {
			/* enable reading if needed */
			if (NULL != skb_queue_front(&sk->rx_queue)) {
				io_sync_enable(&sk->ios, IO_SYNC_READING);
			}

			/* and exit */
			break;
		}
	}

	msg->msg_iov->iov_len = total_len;

	return 0;
}
