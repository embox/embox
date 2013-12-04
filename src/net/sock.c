/**
 * @file
 * @brief Generic socket support routines.
 *
 * @date 12.08.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */
#include <poll.h>
#include <errno.h>
#include <string.h>

#include <util/math.h>

#include <embox/net/family.h>
#include <embox/net/pack.h>
#include <embox/net/sock.h>

#include <hal/ipl.h>
#include <kernel/softirq_lock.h>
#include <fs/idesc_event.h>
#include <mem/misc/pool.h>
#include <net/sock.h>
#include <net/skbuff.h>
#include <net/socket/inet_sock.h>
#include <net/socket/inet6_sock.h>

//TODO this function call from stack (may be place it to other file)
void sock_rcv(struct sock *sk, struct sk_buff *skb,
		unsigned char *p_data, size_t size) {
	if ((sk == NULL) || (skb == NULL) || (p_data == NULL)) {
		return; /* error: invalid argument */
	}

	if (sk->shutdown_flag & (SHUT_RD + 1)) {
		skb_free(skb);
		return; /* error: socket is down */
	}

	skb->p_data = p_data;
	skb->p_data_end = p_data + size;

	skb_queue_push(&sk->rx_queue, skb);
	sk->rx_data_len += size;

	idesc_notify(&sk->idesc, POLLIN);
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

size_t skb_read(struct sk_buff *skb, char *buff, size_t buff_sz);

static int sock_read(struct sock *sk, char *buff, size_t buff_sz, int stream) {
	struct sk_buff *skb;
	size_t len;
	int total_len;

	total_len = 0;

	while(buff_sz) {
		skb = skb_queue_front(&sk->rx_queue);
		if (!skb) {
			goto out;
		}
		len = skb_read(skb, buff, buff_sz);
		if (len == 0) {
			continue;
		}

		buff += len;
		buff_sz -= len;

		if (!stream && skb->p_data == skb->p_data_end) {
			skb_free(skb);
		}
	}

out:
	sk->rx_data_len -= total_len;
	return total_len;
}

extern int sock_wait(struct sock *sk, int flags);

int sock_common_recvmsg(struct sock *sk, struct msghdr *msg, int flags,
		int stream_mode) {
	int res;
	char *buff;
	size_t buff_sz, total_len, len;

	assert(sk);
	assert(msg);
	assert(msg->msg_iov != NULL);
	assert(msg->msg_iov->iov_len);
	assert(msg->msg_iov->iov_base);

	buff = msg->msg_iov->iov_base;
	buff_sz = msg->msg_iov->iov_len;

	total_len = 0;

	softirq_lock();
	{
		while (buff_sz) {
			len = sock_read(sk, buff, buff_sz, stream_mode);

			buff_sz -= len;
			total_len += len;

			if (total_len > 0) {
				msg->msg_iov->iov_len = total_len;
				res = total_len;
				break;
			}
			res = sock_wait(sk, POLLIN | POLLERR);
			if (res != 0) {
				break;
			}
		}
	}
	softirq_unlock();

	return res;
}

in_port_t sock_inet_get_src_port(const struct sock *sk) {
	assert(sk != NULL);
	assert((sk->opt.so_domain == AF_INET)
			|| (sk->opt.so_domain == AF_INET6));

	if (sk->opt.so_domain == AF_INET) {
		return to_const_inet_sock(sk)->src_in.sin_port;
	}

	return to_const_inet6_sock(sk)->src_in6.sin6_port;
}

in_port_t sock_inet_get_dst_port(const struct sock *sk) {
	assert(sk != NULL);
	assert((sk->opt.so_domain == AF_INET)
			|| (sk->opt.so_domain == AF_INET6));

	if (sk->opt.so_domain == AF_INET) {
		return to_const_inet_sock(sk)->dst_in.sin_port;
	}

	return to_const_inet6_sock(sk)->dst_in6.sin6_port;
}
