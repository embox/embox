/**
 * @raw.h
 * @brief Definitions for the RAW-IP module.
 *
 * @date 16.02.2010
 * @author Nikolay Korotky
 */
#ifndef RAW_H
#define RAW_H

#include <net/inet_sock.h>

struct raw_sock {
	/* inet_sock has to be the first member */
	struct inet_sock   inet;
#if 0
	struct icmp_filter filter;
#endif
};

static inline struct raw_sock *raw_sk(const struct sock *sk) {
	return (struct raw_sock *)sk;
}

extern int raw_rcv(struct sk_buff *);

#endif /* RAW_H */
