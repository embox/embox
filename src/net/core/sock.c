/**
 * @file sock.c
 *
 * @details Generic socket support routines.
 * @date 12.08.09
 * @author Nikolay Korotky
 */
#include "string.h"
#include "common.h"
#include "net/skbuff.h"
#include "net/sock.h"
#include "net/udp.h"

int sock_queue_rcv_skb(struct sock *sk, struct sk_buff *skb) {
	return 0;
}

struct sock *sk_alloc(/*struct net *net,*/ int family, gfp_t priority,
                      struct proto *prot) {
	return NULL;
}

void sk_free(struct sock *sk) {
}
