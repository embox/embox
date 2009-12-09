/**
 * @file protocol.c
 *
 * @brief Definitions for the protocol dispatcher.
 * @date 12.08.09
 * @author Nikolay Korotky
 */
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <net/skbuff.h>

#define MAX_INET_PROTOS 256

/* This is used to register protocols. */
struct net_protocol {
    int                     (*handler)(struct sk_buff *pack);
    void                    (*err_handler)(struct sk_buff *pack, int info);
#if 0
    int                     (*gso_send_check)(struct sk_buff *pack);
    struct sk_buff*         (*gso_segment)(struct sk_buff *pack, int features);
    int                     no_policy;
#endif
};

extern struct net_protocol *inet_protos[MAX_INET_PROTOS];

/**
 * Add a protocol handler to the hash tables
 */
extern int inet_add_protocol(struct net_protocol *prot, unsigned char num);

/**
 * Remove a protocol from the hash tables.
 */
extern int inet_del_protocol(struct net_protocol *prot, unsigned char num);

#endif /* PROTOCOL_H_ */
