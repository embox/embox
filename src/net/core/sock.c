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

static struct sock *sk_prot_alloc(struct proto *prot, gfp_t priority,
                int family) {
        //TODO: sock pull
	return NULL;
}

struct sock *sk_alloc(/*struct net *net,*/ int family, gfp_t priority,
                      struct proto *prot) {
        struct sock *sk;
        sk = sk_prot_alloc(prot, 0, family);
        if(sk) {
    		sk->sk_family = family;
    		sk->sk_prot = prot;
	}
	return sk;
}

void sk_free(struct sock *sk) {
	if (sk->sk_destruct) {
                sk->sk_destruct(sk);
        }
}
