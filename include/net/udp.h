/**
 * @file udp.h
 *
 * @brief Definitions for the UDP module.
 * @date 26.03.2009
 * @author Nikolay Korotky
 */
#ifndef UDP_H_
#define UDP_H_

#include "net/inet_sock.h"

struct sock;

typedef struct _udphdr {
        unsigned short source;
        unsigned short dest;
        unsigned short len;
        unsigned short check;
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

/* net/ipv4/udp.c */

extern int udp_init();
extern int udp_rcv(sk_buff_t *);
//extern void udp_err(sk_buff_t *, uint32_t);

#endif /* UDP_H_ */
