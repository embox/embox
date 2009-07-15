/**
 * \file udp.h
 *
 * \date 26.03.2009
 * \author sikmir
 */

#ifndef UDP_H_
#define UDP_H_

#include "inet_sock.h"

typedef struct _udphdr {
        unsigned short source;
        unsigned short dest;
        unsigned short len;
        unsigned short check;
	unsigned char  data[24];
} __attribute__((packed)) udphdr;

#define UDP_HEADER_SIZE	(sizeof(udphdr))

struct udp_sock{
        struct inet_sock inet;
        int              pending;
        unsigned int     corkflag;
	unsigned short   len;
};

static inline struct udp_sock *udp_sk(const struct sock *sk) {
        return (struct udp_sock *)sk;
}

int udp_init(void);

#endif /* UDP_H_ */
