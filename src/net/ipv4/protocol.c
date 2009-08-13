/**
 * \file protocol.c
 * \date 12.08.09
 * \author sikmir
 */

#include "net/protocol.h"
#include "asm/types.h"

struct net_protocol *inet_protos[MAX_INET_PROTOS];

int inet_add_protocol(struct net_protocol *prot, unsigned char protocol) {
        int hash, ret;
        hash = protocol & (MAX_INET_PROTOS - 1);

        if (inet_protos[hash]) {
                ret = -1;
        } else {
                inet_protos[hash] = prot;
                ret = 0;
        }
        return ret;
}

int inet_del_protocol(struct net_protocol *prot, unsigned char protocol) {
        int hash, ret;
        hash = protocol & (MAX_INET_PROTOS - 1);

        if (inet_protos[hash] == prot) {
                inet_protos[hash] = NULL;
                ret = 0;
        } else {
                ret = -1;
        }
        return ret;
}
