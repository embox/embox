/**
 * @file
 * @brief Definitions for the RAW-IP module.
 *
 * @date 16.02.10
 * @author Nikolay Korotky
 */

#ifndef NET_SOCKET_RAW_H_
#define NET_SOCKET_RAW_H_

#include <net/skbuff.h>
#include <stdint.h>

extern const struct sock_proto_ops *const raw_sock_ops;

extern int raw_rcv(struct sk_buff *skb);
extern void raw_err(struct sk_buff *skb, uint32_t info);

#endif /* NET_SOCKET_RAW_H_ */
