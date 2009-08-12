/**
 * \file protocol.c
 * \date 12.08.09
 * \author sikmir
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define MAX_INET_PROTOS 256

struct _net_packet;

/* This is used to register protocols. */
struct net_protocol {
    int                     (*handler)(struct _net_packet *pack);
    void                    (*err_handler)(struct _net_packet *pack, int info);
    int                     (*gso_send_check)(struct _net_packet *pack);
    struct _net_packet     *(*gso_segment)(struct _net_packet *pack, int features);
    int                     no_policy;
};

extern struct net_protocol *inet_protos[MAX_INET_PROTOS];

extern int inet_add_protocol(struct net_protocol *prot, unsigned char num);
extern int inet_del_protocol(struct net_protocol *prot, unsigned char num);

#endif /* PROTOCOL_H_ */
