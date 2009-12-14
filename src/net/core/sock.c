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

static struct udp_sock sk_pool[MAX_SOCK_NUM];
SOCK_INFO sks[MAX_SOCK_NUM];

//TODO:

struct udp_sock* sk_alloc() {
        int i;
        for(i = 0; i < MAX_SOCK_NUM; i++) {
    	        if (0 == sks[i].is_busy) {
                        sks[i].is_busy = 1;
                        sks[i].new_pack = 0;
                        memset(&sk_pool[i], 0, sizeof(sk_pool[i]));
                        sks[i].sk = &sk_pool[i];
                        return sks[i].sk;
                }
        }
        return NULL;
}

void sk_free(struct udp_sock *sk) {
        int i;
        for(i = 0; i < MAX_SOCK_NUM; i++) {
                if (sk == sks[i].sk) {
                        sks[i].is_busy = 0;
                        kfree_skb(sks[i].queue);
                }
        }
}
