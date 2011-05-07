/**
 * @raw.h
 * @brief Definitions for the RAW-IP module.
 *
 * @date 16.02.10
 * @author Nikolay Korotky
 */

#ifndef RAW_H
#define RAW_H

#include <net/inet_sock.h>

typedef struct raw_sock {
	/* inet_sock has to be the first member */
	struct inet_sock   inet;
#if 0
	struct icmp_filter filter;
#endif
} raw_sock_t;

static inline raw_sock_t *raw_sk(const struct sock *sk) {
	return (raw_sock_t *) sk;
}

extern int raw_rcv(struct sk_buff *);

#endif /* RAW_H */
