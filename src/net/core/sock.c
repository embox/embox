/**
 * \file sock.c
 * \date 12.08.09
 * \author sikmir
 * \details Generic socket support routines.
 */

#include "types.h"
#include "common.h"
#include "net/sock.h"
#include "net/udp.h"

SOCK_INFO sks[MAX_SOCK_NUM];

//TODO:

struct udp_sock* sk_alloc() {
        LOG_DEBUG("sk_alloc\n");
        int i;
        for(i = 0; i < MAX_SOCK_NUM; i++) {
    	        if (0 == sks[i].is_busy) {
                        sks[i].is_busy = 1;
                        sks[i].new_pack = 0;
                        memset(&sks[i].sk, 0, sizeof(sks[i].sk));
                        return &sks[i].sk;
                }
        }
        return NULL;
}

void sk_free(struct udp_sock *sk) {
        LOG_DEBUG("sk_free\n");
        int i;
        for(i = 0; i < MAX_SOCK_NUM; i++) {
                if (sk == &sks[i].sk) {
                        sks[i].is_busy = 0;
                        net_packet_free(sks[i].queue);
                }
        }
}
