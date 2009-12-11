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
typedef struct net_protocol {
    int                     (*handler)(sk_buff_t *pack);
    void                    (*err_handler)(sk_buff_t *pack, int info);
#if 0
    int                     (*gso_send_check)(sk_buff_t *pack);
    sk_buff_t              *(*gso_segment)(sk_buff_t *pack, int features);
    int                     no_policy;
#endif
} net_protocol_t;

extern net_protocol_t *inet_protos[MAX_INET_PROTOS];

/**
 * Add a protocol handler to the hash tables
 */
extern int inet_add_protocol(net_protocol_t *prot, unsigned char num);

/**
 * Remove a protocol from the hash tables.
 */
extern int inet_del_protocol(net_protocol_t *prot, unsigned char num);

#endif /* PROTOCOL_H_ */
